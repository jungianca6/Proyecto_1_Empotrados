/*
 * Simulador del robot para probar el mapa sin hardware.
 *
 * Ejecuta el fsm.c REAL (sin modificar) y el mismo nav_events/mapper que
 * corren en el robot. Sólo se reemplaza el hardware:
 *   - robot_move()                    -> mueve un robot virtual en una sala
 *   - sensor_*_obstacle_detected()    -> rayos digitales contra paredes/cajas
 *   - clock_gettime()                 -> reloj virtual (corre mucho más rápido)
 *
 * Uso: map_sim [--secs N] [--seed N] [--mismatch] [--out map.json]
 *              [--log events.log] [--ascii]
 *
 * --mismatch hace que el robot "real" difiera del modelo del mapa
 * (más rápido, gira menos, sensor más corto) para ver la deriva.
 */
#define _GNU_SOURCE
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fsm.h"
#include "mapper.h"
#include "motors.h"
#include "nav_events.h"
#include "sensors.h"

#define PI 3.14159265358979323846

/* ------------------------------------------------------------ reloj virtual */

static uint64_t vt_ns;

int clock_gettime(clockid_t id, struct timespec *ts)
{
    (void)id;
    ts->tv_sec  = (time_t)(vt_ns / 1000000000ULL);
    ts->tv_nsec = (long)(vt_ns % 1000000000ULL);
    return 0;
}

/* -------------------------------------------------------------------- sala */

typedef struct { double x0, y0, x1, y1; } rect_t;

static const double RX0 = -150, RX1 = 150, RY0 = -100, RY1 = 140;   /* cm */
static const rect_t boxes[] = { {-90, 30, -50, 70}, {40, -70, 90, -40} };
#define NBOX ((int)(sizeof(boxes) / sizeof(boxes[0])))

static int blocked(double x, double y)
{
    if (x < RX0 || x > RX1 || y < RY0 || y > RY1)
        return 1;
    for (int i = 0; i < NBOX; i++)
        if (x >= boxes[i].x0 && x <= boxes[i].x1 && y >= boxes[i].y0 && y <= boxes[i].y1)
            return 1;
    return 0;
}

static double dist_to_obstacles(double x, double y)
{
    double d = fmin(fmin(x - RX0, RX1 - x), fmin(y - RY0, RY1 - y));

    for (int i = 0; i < NBOX; i++) {
        double dx = fmax(fmax(boxes[i].x0 - x, 0.0), x - boxes[i].x1);
        double dy = fmax(fmax(boxes[i].y0 - y, 0.0), y - boxes[i].y1);
        d = fmin(d, hypot(dx, dy));
    }
    return d;
}

/* ------------------------------------------------------------ robot "real" */

static double tx, ty, tth = PI / 2;          /* pose verdadera */
static double tv, trate, toff, tF, tS, trad; /* parámetros verdaderos */
static RobotDirection g_dir = ROBOT_STOP;
static long wall_pushes;    /* pasos de simulación en que una pared frenó al robot */

void robot_move(RobotDirection dir, int speed_pct)
{
    g_dir = (speed_pct <= 0) ? ROBOT_STOP : dir;
}

static int ray_hit(double ang, double range)
{
    for (double s = 0.0; s <= range; s += 1.0)
        if (blocked(tx + s * cos(ang), ty + s * sin(ang)))
            return 1;
    return 0;
}

int sensor_obstacle_detected(void)      { return ray_hit(tth, tF); }
int sensor_side_obstacle_detected(void) { return ray_hit(tth + PI / 2, tS); }
void led_obstacle_set(int on)           { (void)on; }

static void rotate_about(double wx, double wy, double a)
{
    double dx = tx - wx, dy = ty - wy;

    tx = wx + dx * cos(a) - dy * sin(a);
    ty = wy + dx * sin(a) + dy * cos(a);
    tth += a;
}

static void phys_step(double dt)
{
    switch (g_dir) {
    case ROBOT_FORWARD:
        tx += tv * dt * cos(tth);
        ty += tv * dt * sin(tth);
        break;
    case ROBOT_BACKWARD:
        tx -= tv * dt * cos(tth);
        ty -= tv * dt * sin(tth);
        break;
    case ROBOT_TURN_RIGHT: {            /* rueda derecha quieta */
        double wx = tx + toff * sin(tth), wy = ty - toff * cos(tth);
        rotate_about(wx, wy, -trate * dt);
        break;
    }
    case ROBOT_TURN_LEFT: {             /* rueda izquierda quieta */
        double wx = tx - toff * sin(tth), wy = ty + toff * cos(tth);
        rotate_about(wx, wy, +trate * dt);
        break;
    }
    default:
        break;
    }

    double cx = fmin(fmax(tx, RX0 + 5.0), RX1 - 5.0);
    double cy = fmin(fmax(ty, RY0 + 5.0), RY1 - 5.0);
    if (cx != tx || cy != ty)
        wall_pushes++;      /* el robot real choco con una pared sin sensor */
    tx = cx;
    ty = cy;
}

/* ---------------------------------------------------------------- métricas */

static mapper_params_t mp;
static uint8_t *tv_grid;

static int col_of(double x) { return (int)floor(x / mp.cell_cm) + mp.grid_w / 2; }
static int row_of(double y) { return mp.grid_h / 2 - 1 - (int)floor(y / mp.cell_cm); }

static void stamp_true(void)
{
    double R = trad;

    for (int r = row_of(ty + R); r <= row_of(ty - R); r++)
        for (int c = col_of(tx - R); c <= col_of(tx + R); c++) {
            if (r < 0 || r >= mp.grid_h || c < 0 || c >= mp.grid_w)
                continue;
            double cx = (c - mp.grid_w / 2 + 0.5) * mp.cell_cm;
            double cy = (mp.grid_h / 2 - 1 - r + 0.5) * mp.cell_cm;
            if (hypot(cx - tx, cy - ty) <= R)
                tv_grid[r * mp.grid_w + c] = 1;
        }
}

static double ang_diff_deg(double a, double b)
{
    double d = fmod(a - b, 360.0);

    if (d > 180.0)  d -= 360.0;
    if (d < -180.0) d += 360.0;
    return d;
}

static void pose_error(double *pos_cm, double *ang_deg)
{
    double ex, ey, eth;

    mapper_get_pose(&ex, &ey, &eth);
    *pos_cm  = hypot(ex - tx, ey - ty);
    *ang_deg = ang_diff_deg(eth, fmod(tth / (PI / 180.0) + 3600.0, 360.0));
}

int main(int argc, char **argv)
{
    int secs = 120, seed = 1, mismatch = 0, ascii = 0;
    const char *out = "sim_map.json", *logp = "sim_events.log";

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "--secs") && i + 1 < argc)  secs = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--seed") && i + 1 < argc)  seed = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--out") && i + 1 < argc)   out = argv[++i];
        else if (!strcmp(argv[i], "--log") && i + 1 < argc)   logp = argv[++i];
        else if (!strcmp(argv[i], "--mismatch"))              mismatch = 1;
        else if (!strcmp(argv[i], "--ascii"))                 ascii = 1;
    }

    mapper_params_default(&mp);
    tv = mp.speed_cm_s;  trate = mp.turn_rate_deg_s * PI / 180.0;  toff = mp.pivot_offset_cm;
    tF = mp.front_obstacle_cm;  tS = mp.side_obstacle_cm;  trad = mp.robot_radius_cm;
    if (mismatch) {
        tv *= 1.12;         /* avanza 12 % más rápido que el modelo */
        trate *= 0.85;      /* gira 15 % menos */
        tF *= 0.90;         /* el IR dispara 10 % más cerca */
    }

    tv_grid = calloc((size_t)mp.grid_w * mp.grid_h, 1);
    vt_ns = 100000000000ULL + (uint64_t)seed * 7919000ULL;

    fsm_init();
    if (nav_events_init(logp, out, "") != 0) {
        fprintf(stderr, "nav_events_init fallo\n");
        return 1;
    }
    fsm_update(EVENT_START);

    long steps = (long)secs * 100;      /* paso de 10 ms, igual que main.c */
    for (long i = 1; i <= steps; i++) {
        vt_ns += 10000000ULL;
        phys_step(0.01);
        if (g_dir != ROBOT_STOP)
            stamp_true();
        fsm_update(EVENT_NONE);
        nav_events_tick();

        if (i % 3000 == 0) {
            double pe, ae;
            pose_error(&pe, &ae);
            printf("  t=%4lds  error de pose: %6.1f cm, %6.1f grados\n", i / 100, pe, ae);
        }
    }

    fsm_update(EVENT_STOP);
    nav_events_tick();

    /* ---- resumen ---- */
    int w, h;
    const uint8_t *g = mapper_grid(&w, &h);
    int free_cells = 0, true_cov = 0, est_vis = 0, est_vis_ok = 0, est_obs = 0, est_obs_ok = 0;

    for (int r = 0; r < h; r++)
        for (int c = 0; c < w; c++) {
            double cx = (c - w / 2 + 0.5) * mp.cell_cm;
            double cy = (h / 2 - 1 - r + 0.5) * mp.cell_cm;
            int idx = r * w + c;
            if (!blocked(cx, cy)) {
                free_cells++;
                if (tv_grid[idx]) true_cov++;
            }
            if (g[idx] == MAP_VISITED) {
                est_vis++;
                if (tv_grid[idx]) est_vis_ok++;
            }
            if (g[idx] == MAP_OBSTACLE) {
                est_obs++;
                if (dist_to_obstacles(cx, cy) <= 1.5 * mp.cell_cm) est_obs_ok++;
            }
        }

    double pe, ae;
    pose_error(&pe, &ae);

    printf("\nEscenario: %s, %d s virtuales, semilla %d\n",
           mismatch ? "parametros DESAJUSTADOS" : "parametros iguales al modelo", secs, seed);
    printf("  cobertura real de la sala:        %5.1f %%\n", 100.0 * true_cov / free_cells);
    printf("  celdas visitadas en el mapa:      %d (%.1f %% coinciden con lo realmente recorrido)\n",
           est_vis, est_vis ? 100.0 * est_vis_ok / est_vis : 0.0);
    printf("  celdas de obstaculo en el mapa:   %d (%.1f %% cerca de una pared o caja real)\n",
           est_obs, est_obs ? 100.0 * est_obs_ok / est_obs : 0.0);
    printf("  error de pose final:              %.1f cm, %.1f grados\n", pe, ae);
    printf("  pose verdadera final:             (%.1f, %.1f) cm, %.1f grados\n",
           tx, ty, fmod(tth / (PI / 180.0) + 3600.0, 360.0));
    printf("  pasos en que una pared frenó al robot sin sensor: %ld (%.1f s)\n",
           wall_pushes, wall_pushes / 100.0);

    if (ascii) {
        printf("\n");
        mapper_dump_ascii(stdout);
    }

    nav_events_close();
    free(tv_grid);
    return 0;
}
