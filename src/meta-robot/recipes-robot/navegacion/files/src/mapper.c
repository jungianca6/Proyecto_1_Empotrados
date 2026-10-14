#include "mapper.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define PI      3.14159265358979323846
#define DEG2RAD (PI / 180.0)

static mapper_params_t P;
static uint8_t *grid;
static double px, py, th;      /* pose estimada: cm, cm, rad */
static map_motion_t motion;
static uint64_t last_t;
static int have_t, dirty;
static unsigned seq, oob;

/* ---------------------------------------------------------------- params */

void mapper_params_default(mapper_params_t *p)
{
    /* Valores PROVISIONALES: hay que calibrarlos con el robot real. */
    p->cell_cm           = 5.0;
    p->grid_w            = 80;
    p->grid_h            = 80;
    p->speed_cm_s        = 20.0;
    p->turn_rate_deg_s   = 80.0;
    p->pivot_offset_cm   = 7.5;
    p->robot_radius_cm   = 10.0;
    p->front_obstacle_cm = 20.0;
    p->side_obstacle_cm  = 15.0;
    p->turn_sign         = 1;
}

int mapper_params_load(mapper_params_t *p, const char *path)
{
    FILE *f = fopen(path, "r");
    char line[160];

    if (!f)
        return -1;

    while (fgets(line, sizeof(line), f)) {
        char *s = line, *eq, *e;

        while (*s == ' ' || *s == '\t')
            s++;
        if (*s == '#' || *s == '\n' || *s == '\r' || *s == '\0')
            continue;

        eq = strchr(s, '=');
        if (!eq)
            continue;
        *eq = '\0';
        for (e = eq - 1; e >= s && (*e == ' ' || *e == '\t'); e--)
            *e = '\0';

        double v = atof(eq + 1);

        if      (!strcmp(s, "cell_cm"))           p->cell_cm = v;
        else if (!strcmp(s, "grid_w"))            p->grid_w = (int)v;
        else if (!strcmp(s, "grid_h"))            p->grid_h = (int)v;
        else if (!strcmp(s, "speed_cm_s"))        p->speed_cm_s = v;
        else if (!strcmp(s, "turn_rate_deg_s"))   p->turn_rate_deg_s = v;
        else if (!strcmp(s, "pivot_offset_cm"))   p->pivot_offset_cm = v;
        else if (!strcmp(s, "robot_radius_cm"))   p->robot_radius_cm = v;
        else if (!strcmp(s, "front_obstacle_cm")) p->front_obstacle_cm = v;
        else if (!strcmp(s, "side_obstacle_cm"))  p->side_obstacle_cm = v;
        else if (!strcmp(s, "turn_sign"))         p->turn_sign = (v < 0) ? -1 : 1;
    }

    fclose(f);
    return 0;
}

/* ------------------------------------------------------------------ grid */

static int col_of(double x) { return (int)floor(x / P.cell_cm) + P.grid_w / 2; }
static int row_of(double y) { return P.grid_h / 2 - 1 - (int)floor(y / P.cell_cm); }

static int in_bounds(int r, int c)
{
    return r >= 0 && r < P.grid_h && c >= 0 && c < P.grid_w;
}

/* Marca como visitadas las celdas bajo el robot; nunca borra obstáculos. */
static void stamp_visited(double x, double y)
{
    double R = P.robot_radius_cm;
    int r0 = row_of(y + R), r1 = row_of(y - R);
    int c0 = col_of(x - R), c1 = col_of(x + R);

    if (!in_bounds(row_of(y), col_of(x)))
        oob++;

    for (int r = r0; r <= r1; r++) {
        for (int c = c0; c <= c1; c++) {
            if (!in_bounds(r, c))
                continue;
            double cx = (c - P.grid_w / 2 + 0.5) * P.cell_cm;
            double cy = (P.grid_h / 2 - 1 - r + 0.5) * P.cell_cm;
            if (hypot(cx - x, cy - y) <= R) {
                uint8_t *g = &grid[r * P.grid_w + c];
                if (*g == MAP_UNKNOWN) {
                    *g = MAP_VISITED;
                    dirty = 1;
                }
            }
        }
    }
}

static void mark_obstacle(double x, double y)
{
    int r = row_of(y), c = col_of(x);

    if (!in_bounds(r, c)) {
        oob++;
        return;
    }
    if (grid[r * P.grid_w + c] != MAP_OBSTACLE) {
        grid[r * P.grid_w + c] = MAP_OBSTACLE;
        dirty = 1;
    }
}

/* ------------------------------------------------------------ odometría */

static void integrate(double dt)
{
    if (motion == MAP_MOVE_NONE || dt <= 0.0)
        return;

    if (motion == MAP_MOVE_FORWARD || motion == MAP_MOVE_BACKWARD) {
        double dist = P.speed_cm_s * dt * (motion == MAP_MOVE_FORWARD ? 1.0 : -1.0);
        int n = (int)ceil(fabs(dist) / (P.cell_cm * 0.5));
        if (n < 1) n = 1;
        double sx = cos(th) * dist / n, sy = sin(th) * dist / n;

        for (int i = 0; i < n; i++) {
            px += sx;
            py += sy;
            stamp_visited(px, py);
        }
        return;
    }

    /*
     * Giro sobre una rueda: sin PWM, una rueda gira y la otra queda
     * quieta, así que el centro del chasis describe un arco alrededor
     * de la rueda quieta.
     */
    int left = (motion == MAP_MOVE_TURN_LEFT);
    if (P.turn_sign < 0)
        left = !left;

    double dth  = P.turn_rate_deg_s * DEG2RAD * dt * (left ? 1.0 : -1.0);
    double side = th + (left ? PI / 2 : -PI / 2);        /* rueda quieta */
    double wx = px + P.pivot_offset_cm * cos(side);
    double wy = py + P.pivot_offset_cm * sin(side);
    double rx = px - wx, ry = py - wy, th0 = th;
    int n = (int)ceil(fabs(dth) / (5.0 * DEG2RAD));
    if (n < 1) n = 1;

    for (int i = 1; i <= n; i++) {
        double a = dth * i / n;
        px = wx + rx * cos(a) - ry * sin(a);
        py = wy + rx * sin(a) + ry * cos(a);
        th = th0 + a;
        stamp_visited(px, py);
    }
}

/* ------------------------------------------------------------------- API */

int mapper_init(const mapper_params_t *p)
{
    if (p)
        P = *p;
    else
        mapper_params_default(&P);

    if (P.cell_cm <= 0.0)                       P.cell_cm = 5.0;
    if (P.grid_w < 8 || P.grid_w > 400)         P.grid_w = 80;
    if (P.grid_h < 8 || P.grid_h > 400)         P.grid_h = 80;

    free(grid);
    grid = calloc((size_t)P.grid_w * P.grid_h, 1);
    if (!grid)
        return -1;

    px = py = 0.0;
    th = PI / 2;
    motion = MAP_MOVE_NONE;
    have_t = 0;
    last_t = 0;
    seq = 0;
    oob = 0;
    dirty = 1;
    stamp_visited(px, py);
    return 0;
}

void mapper_free(void)
{
    free(grid);
    grid = NULL;
}

void mapper_advance(uint64_t t_ms)
{
    if (!grid)
        return;
    if (!have_t) {
        last_t = t_ms;
        have_t = 1;
        return;
    }
    if (t_ms <= last_t)
        return;

    double dt = (double)(t_ms - last_t) / 1000.0;
    last_t = t_ms;
    integrate(dt);
}

void mapper_set_motion(uint64_t t_ms, map_motion_t m)
{
    mapper_advance(t_ms);
    motion = m;
}

void mapper_obstacle(uint64_t t_ms, int front, int left)
{
    if (!grid)
        return;
    mapper_advance(t_ms);

    if (front)
        mark_obstacle(px + P.front_obstacle_cm * cos(th),
                      py + P.front_obstacle_cm * sin(th));
    if (left)
        mark_obstacle(px + P.side_obstacle_cm * cos(th + PI / 2),
                      py + P.side_obstacle_cm * sin(th + PI / 2));
}

map_motion_t mapper_motion_from_name(const char *s)
{
    if (!s)                              return MAP_MOVE_NONE;
    if (!strcmp(s, "MOVING_FORWARD"))    return MAP_MOVE_FORWARD;
    if (!strcmp(s, "BACKING_UP"))        return MAP_MOVE_BACKWARD;
    if (!strcmp(s, "TURNING_LEFT"))      return MAP_MOVE_TURN_LEFT;
    if (!strcmp(s, "TURNING_RIGHT"))     return MAP_MOVE_TURN_RIGHT;
    return MAP_MOVE_NONE;
}

int mapper_dirty(void) { return dirty; }

void mapper_get_params(mapper_params_t *p) { *p = P; }

void mapper_get_pose(double *x, double *y, double *theta_deg)
{
    double d = fmod(th / DEG2RAD, 360.0);
    if (d < 0) d += 360.0;
    if (x) *x = px;
    if (y) *y = py;
    if (theta_deg) *theta_deg = d;
}

const uint8_t *mapper_grid(int *w, int *h)
{
    if (w) *w = P.grid_w;
    if (h) *h = P.grid_h;
    return grid;
}

int mapper_write_json(const char *path)
{
    char tmp[300];
    unsigned visited = 0, obstacles = 0;
    double x, y, deg;
    FILE *f;

    if (!grid)
        return -1;

    snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    f = fopen(tmp, "w");
    if (!f)
        return -1;

    for (int i = 0; i < P.grid_w * P.grid_h; i++) {
        if (grid[i] == MAP_VISITED)  visited++;
        if (grid[i] == MAP_OBSTACLE) obstacles++;
    }
    mapper_get_pose(&x, &y, &deg);
    seq++;

    fprintf(f,
        "{\"seq\":%u,\"cell_cm\":%.1f,\"width\":%d,\"height\":%d,"
        "\"origin\":{\"col\":%d,\"row\":%d},"
        "\"robot\":{\"x\":%.1f,\"y\":%.1f,\"theta_deg\":%.1f,\"col\":%d,\"row\":%d},"
        "\"stats\":{\"visited\":%u,\"obstacles\":%u,\"out_of_bounds\":%u},"
        "\"grid\":[",
        seq, P.cell_cm, P.grid_w, P.grid_h,
        col_of(0.0), row_of(0.0),
        x, y, deg, col_of(x), row_of(y),
        visited, obstacles, oob);

    for (int r = 0; r < P.grid_h; r++) {
        fputc(r ? ',' : ' ', f);
        fputc('[', f);
        for (int c = 0; c < P.grid_w; c++) {
            if (c) fputc(',', f);
            fputc('0' + grid[r * P.grid_w + c], f);
        }
        fputc(']', f);
    }
    fputs("]}\n", f);

    if (fclose(f) != 0)
        return -1;
    if (rename(tmp, path) != 0)
        return -1;

    dirty = 0;
    return 0;
}

void mapper_dump_ascii(FILE *f)
{
    int r0 = P.grid_h, r1 = -1, c0 = P.grid_w, c1 = -1;

    if (!grid)
        return;

    for (int r = 0; r < P.grid_h; r++)
        for (int c = 0; c < P.grid_w; c++)
            if (grid[r * P.grid_w + c]) {
                if (r < r0) r0 = r;
                if (r > r1) r1 = r;
                if (c < c0) c0 = c;
                if (c > c1) c1 = c;
            }

    if (r1 < 0) {
        fputs("(mapa vacio)\n", f);
        return;
    }

    int rr = row_of(py), rc = col_of(px);
    if (in_bounds(rr, rc)) {
        if (rr < r0) r0 = rr;
        if (rr > r1) r1 = rr;
        if (rc < c0) c0 = rc;
        if (rc > c1) c1 = rc;
    }

    for (int r = r0; r <= r1; r++) {
        for (int c = c0; c <= c1; c++) {
            char ch = ' ';
            uint8_t v = grid[r * P.grid_w + c];
            if (v == MAP_VISITED)  ch = '.';
            if (v == MAP_OBSTACLE) ch = '#';
            if (r == rr && c == rc) ch = 'R';
            fputc(ch, f);
        }
        fputc('\n', f);
    }
}
