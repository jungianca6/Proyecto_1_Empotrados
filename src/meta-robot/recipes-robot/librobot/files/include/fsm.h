#ifndef FSM_H
#define FSM_H

#include "motors.h"

/*
 * Estados de navegación del robot.
 *
 * El algoritmo implementa cobertura reactiva con rebote:
 * el robot avanza, detecta obstáculos, se detiene,
 * retrocede y cambia de dirección antes de continuar.
 */
typedef enum {
    STATE_INIT = 0,
    STATE_IDLE,
    STATE_MOVING_FORWARD,
    STATE_AVOID_STOP,
    STATE_BACKING_UP,
    STATE_TURNING_LEFT,
    STATE_TURNING_RIGHT,
    STATE_STOPPED,
    STATE_EMERGENCY
} RobotState;

typedef enum {
    EVENT_NONE = 0,
    EVENT_START,
    EVENT_TIMEOUT,
    EVENT_OBSTACLE,
    EVENT_STOP
} RobotEvent;

void fsm_init(void);
void fsm_update(RobotEvent event);
RobotState fsm_get_state(void);

#endif // FSM_H