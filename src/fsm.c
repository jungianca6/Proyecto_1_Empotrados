#include <stdio.h>
#include <time.h>
#include "fsm.h"
#include "sensors.h"

static RobotState current_state = STATE_INIT;
static unsigned long state_entry_time = 0;

static unsigned long get_millis(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

void fsm_init(void) {
    current_state = STATE_IDLE;
    state_entry_time = get_millis();
    robot_move(ROBOT_STOP, 0);
    printf("[FSM] Inicializada en estado: IDLE\n");
}

RobotState fsm_get_state(void) {
    return current_state;
}

void fsm_update(RobotEvent event) {
    unsigned long now = get_millis();
    unsigned long elapsed = now - state_entry_time;

    // 1. EVALUAR SENSOR EN TIEMPO REAL
    if (sensor_obstacle_detected()) {
        event = EVENT_OBSTACLE;
    }

    // 2. PARADA FORZADA SI HAY EVENTO EXPLICITO DE STOP
    if (event == EVENT_STOP) {
        current_state = STATE_STOPPED;
        robot_move(ROBOT_STOP, 0);
        printf("[FSM] Parada forzada. Estado: STOPPED\n");
        return;
    }

    // 3. MAQUINA DE ESTADOS FINITOS
    switch (current_state) {
        case STATE_IDLE:
            if (event == EVENT_START) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;
                robot_move(ROBOT_FORWARD, 100);
                printf("[FSM] Transición: IDLE -> MOVING_FORWARD\n");
            }
            break;

        case STATE_MOVING_FORWARD:
            // Si detecta un obstáculo mientras avanza, pasa a esquivar inmediatamente
            if (event == EVENT_OBSTACLE) {
                current_state = STATE_TURNING_RIGHT;
                state_entry_time = now;
                robot_move(ROBOT_TURN_RIGHT, 100);
                printf("[FSM ALERTA] ¡Obstáculo detectado! Esquivando a la derecha...\n");
            }
            break;

        case STATE_TURNING_RIGHT:
            // Mantiene el giro por 1.5s (1500 ms) para desviar el chasis
            if (elapsed >= 1500) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;
                robot_move(ROBOT_FORWARD, 100);
                printf("[FSM] Maniobra completada. Reanudando avance en línea recta...\n");
            }
            break;

        case STATE_STOPPED:
            robot_move(ROBOT_STOP, 0);
            break;

        default:
            break;
    }
}