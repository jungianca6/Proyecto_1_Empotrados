#include <stdio.h>
#include <time.h>
#include "fsm.h"

static RobotState current_state = STATE_INIT;
static unsigned long state_entry_time = 0;

// Obtener tiempo actual en milisegundos con reloj monotónico del kernel
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

    // Interrupción prioritaria de seguridad
    if (event == EVENT_OBSTACLE || event == EVENT_STOP) {
        current_state = STATE_STOPPED;
        robot_move(ROBOT_STOP, 0);
        printf("[FSM] Evento de parada forzada. Estado: STOPPED\n");
        return;
    }

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
            if (elapsed >= 3000) { // Avanza durante 3000 ms (3s)
                current_state = STATE_TURNING_LEFT;
                state_entry_time = now;
                robot_move(ROBOT_TURN_LEFT, 100);
                printf("[FSM] Transición: MOVING_FORWARD -> TURNING_LEFT\n");
            }
            break;

        case STATE_TURNING_LEFT:
            if (elapsed >= 1500) { // Gira a la izquierda por 1.5s
                current_state = STATE_TURNING_RIGHT;
                state_entry_time = now;
                robot_move(ROBOT_TURN_RIGHT, 100);
                printf("[FSM] Transición: TURNING_LEFT -> TURNING_RIGHT\n");
            }
            break;

        case STATE_TURNING_RIGHT:
            if (elapsed >= 1500) { // Gira a la derecha por 1.5s
                current_state = STATE_STOPPED;
                state_entry_time = now;
                robot_move(ROBOT_STOP, 0);
                printf("[FSM] Transición: TURNING_RIGHT -> STOPPED\n");
            }
            break;

        case STATE_STOPPED:
            // Estado final sin acción pendiente
            break;

        default:
            break;
    }
}