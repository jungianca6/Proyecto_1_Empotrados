#include "nav_events.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fsm.h"
#include "mapper.h"
#include "sensors.h"

#define DEFAULT_LOG   "/tmp/robot_events.log"
#define DEFAULT_MAP   "/tmp/robot_map.json"
#define DEFAULT_CONF  "/etc/robot_map.conf"

#define LOG_MAX_BYTES   (1024L * 1024L)   /* al pasarse, el log rota a .1 */
#define MAP_WRITE_MS    500

static FILE *log_f;
static char log_path[200];
static char map_path[200];
static RobotState prev_state;
static uint64_t t0, last_write;

static uint64_t now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
}

static const char *state_name(RobotState s)
{
    switch (s) {
    case STATE_INIT:           return "INIT";
    case STATE_IDLE:           return "IDLE";
    case STATE_MOVING_FORWARD: return "MOVING_FORWARD";
    case STATE_AVOID_STOP:     return "AVOID_STOP";
    case STATE_BACKING_UP:     return "BACKING_UP";
    case STATE_TURNING_LEFT:   return "TURNING_LEFT";
    case STATE_TURNING_RIGHT:  return "TURNING_RIGHT";
    case STATE_STOPPED:        return "STOPPED";
    case STATE_EMERGENCY:      return "EMERGENCY";
    default:                   return "UNKNOWN";
    }
}

static void log_line(const char *fmt, ...)
{
    va_list ap;

    if (!log_f)
        return;

    va_start(ap, fmt);
    vfprintf(log_f, fmt, ap);
    va_end(ap);
    fputc('\n', log_f);
    fflush(log_f);

    if (ftell(log_f) > LOG_MAX_BYTES) {
        char old[220];

        fclose(log_f);
        snprintf(old, sizeof(old), "%s.1", log_path);
        rename(log_path, old);
        log_f = fopen(log_path, "w");
        if (log_f)
            fprintf(log_f, "{\"t\":%llu,\"ev\":\"rotated\"}\n",
                    (unsigned long long)(now_ms() - t0));
    }
}

int nav_events_init(const char *log_p, const char *map_p, const char *conf_p)
{
    mapper_params_t mp;

    snprintf(log_path, sizeof(log_path), "%s", log_p ? log_p : DEFAULT_LOG);
    snprintf(map_path, sizeof(map_path), "%s", map_p ? map_p : DEFAULT_MAP);

    mapper_params_default(&mp);
    if (!conf_p)
        conf_p = DEFAULT_CONF;
    if (conf_p[0])
        mapper_params_load(&mp, conf_p);

    if (mapper_init(&mp) != 0)
        return -1;

    t0 = now_ms();
    last_write = 0;
    prev_state = fsm_get_state();
    mapper_advance(0);

    log_f = fopen(log_path, "w");
    log_line("{\"t\":0,\"ev\":\"start\",\"params\":{\"cell_cm\":%.1f,\"speed_cm_s\":%.1f,"
             "\"turn_rate_deg_s\":%.1f,\"pivot_offset_cm\":%.1f,\"robot_radius_cm\":%.1f,"
             "\"front_obstacle_cm\":%.1f,\"side_obstacle_cm\":%.1f,\"turn_sign\":%d}}",
             mp.cell_cm, mp.speed_cm_s, mp.turn_rate_deg_s, mp.pivot_offset_cm,
             mp.robot_radius_cm, mp.front_obstacle_cm, mp.side_obstacle_cm, mp.turn_sign);

    mapper_write_json(map_path);
    return 0;
}

void nav_events_tick(void)
{
    uint64_t t = now_ms() - t0;
    RobotState s = fsm_get_state();

    /* Integra con el movimiento ANTERIOR hasta este instante. */
    mapper_advance(t);

    if (s != prev_state) {
        const char *to = state_name(s);

        log_line("{\"t\":%llu,\"ev\":\"state\",\"from\":\"%s\",\"to\":\"%s\"}",
                 (unsigned long long)t, state_name(prev_state), to);

        mapper_set_motion(t, mapper_motion_from_name(to));

        if (prev_state == STATE_MOVING_FORWARD && s == STATE_AVOID_STOP) {
            int front = sensor_obstacle_detected();
            int left  = sensor_side_obstacle_detected();
            double x, y, deg;

            mapper_obstacle(t, front, left);
            mapper_get_pose(&x, &y, &deg);
            log_line("{\"t\":%llu,\"ev\":\"obstacle\",\"front\":%d,\"left\":%d,"
                     "\"x\":%.1f,\"y\":%.1f,\"th\":%.1f}",
                     (unsigned long long)t, front, left, x, y, deg);
        }

        prev_state = s;
    }

    if (t - last_write >= MAP_WRITE_MS && mapper_dirty()) {
        mapper_write_json(map_path);
        last_write = t;
    }
}

void nav_events_close(void)
{
    uint64_t t = now_ms() - t0;

    mapper_advance(t);
    mapper_write_json(map_path);
    log_line("{\"t\":%llu,\"ev\":\"end\"}", (unsigned long long)t);

    if (log_f) {
        fclose(log_f);
        log_f = NULL;
    }
    mapper_free();
}
