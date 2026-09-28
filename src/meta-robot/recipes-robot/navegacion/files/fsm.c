#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "fsm.h"
#include "sensors.h"

static RobotState current_state = STATE_INIT;
static unsigned long state_entry_time = 0;

static void play_notification(const char *variable, const char *fallback)
{
    const char *path = getenv(variable);
    pid_t child;

    if (path == NULL) {
        path = fallback;
    }
    child = fork();
    if (child == 0) {
        execl("/usr/bin/audioctl", "audioctl", "notify", path, (char *)NULL);
        _exit(127);
    }
    if (child > 0) {
        waitpid(child, NULL, 0);
    }
}

static unsigned long get_millis(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

void fsm_init(void) {
    current_state = STATE_IDLE;
    state_entry_time = get_millis();
    robot_move(ROBOT_STOP, 0);
    play_notification("ROBOT_AUDIO_SYSTEM_START",
                      "/usr/share/robot-audio/system-start.mp3");
}

RobotState fsm_get_state(void) {
    return current_state;
}

void fsm_update(RobotEvent event) {
    unsigned long now = get_millis();
    unsigned long elapsed = now - state_entry_time;

    // Evaluación del sensor en tiempo real
    if (sensor_obstacle_detected()) {
        event = EVENT_OBSTACLE;
    }

    if (event == EVENT_STOP) {
        current_state = STATE_STOPPED;
        robot_move(ROBOT_STOP, 0);
        return;
    }

    if (event == EVENT_MANUAL) {
        play_notification("ROBOT_AUDIO_MANUAL",
                          "/usr/share/robot-audio/manual-mode.mp3");
        return;
    }

    switch (current_state) {
        case STATE_IDLE:
            if (event == EVENT_START) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;
                robot_move(ROBOT_FORWARD, 100);
                play_notification("ROBOT_AUDIO_AUTONOMOUS_START",
                                  "/usr/share/robot-audio/autonomous-start.mp3");
            }
            break;

        case STATE_MOVING_FORWARD:
            // Al detectar obstáculo, cambia a esquivar
            if (event == EVENT_OBSTACLE) {
                current_state = STATE_TURNING_RIGHT;
                state_entry_time = now;
                robot_move(ROBOT_TURN_RIGHT, 100);
                play_notification("ROBOT_AUDIO_OBSTACLE",
                                  "/usr/share/robot-audio/obstacle.mp3");
            }
            break;

        case STATE_TURNING_RIGHT:
            // Giro corto: 700 ms (0.7 segundos) para desviarse un poco
            if (elapsed >= 700) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;
                robot_move(ROBOT_FORWARD, 100); // Reanuda avance inmediatamente
            }
            break;

        case STATE_STOPPED:
            robot_move(ROBOT_STOP, 0);
            break;

        default:
            break;
    }
}