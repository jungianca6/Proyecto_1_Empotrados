#include <stdlib.h>
#include <time.h>

#include "fsm.h"
#include "sensors.h"

/*
 * Algoritmo de cobertura reactiva con rebote
 * ------------------------------------------
 *
 * El robot avanza mientras el camino esté despejado.
 *
 * Cuando detecta un obstáculo:
 * 1. Se detiene brevemente.
 * 2. Retrocede para alejarse.
 * 3. Elige un giro hacia izquierda o derecha.
 * 4. Gira durante un intervalo variable.
 * 5. Continúa avanzando.
 *
 * Para reducir ciclos repetitivos:
 * - la dirección del giro cambia de forma pseudoaleatoria;
 * - si gira varias veces seguidas hacia el mismo lado,
 *   se fuerza el giro contrario;
 * - la duración del giro varía ligeramente.
 */

#define STOP_TIME_MS       200
#define BACKWARD_TIME_MS   500

#define TURN_MIN_TIME_MS   500
#define TURN_RANDOM_MS     600

#define MOTOR_SPEED        100

static RobotState current_state = STATE_INIT;
static unsigned long state_entry_time = 0;

static unsigned long turn_duration = 700;

/*
 * Información de giros anteriores:
 * -1 = izquierda
 *  1 = derecha
 *  0 = todavía no se ha realizado ningún giro
 */
static int last_turn = 0;
static int repeated_turns = 0;

static unsigned long get_millis(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000UL) +
           (ts.tv_nsec / 1000000UL);
}

/*
 * Selecciona la dirección de giro.
 *
 * Normalmente se elige de forma pseudoaleatoria.
 * Si el robot ha repetido dos veces el mismo sentido,
 * se fuerza el giro contrario para reducir loops.
 */
static int choose_turn_direction(void)
{
    int direction;

    if (repeated_turns >= 2 && last_turn != 0) {
        direction = -last_turn;
        repeated_turns = 0;
    }
    else {
        direction = (rand() % 2) ? 1 : -1;
    }

    if (direction == last_turn) {
        repeated_turns++;
    }
    else {
        repeated_turns = 1;
    }

    last_turn = direction;

    return direction;
}

void fsm_init(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)ts.tv_nsec);

    current_state = STATE_IDLE;
    state_entry_time = get_millis();

    robot_move(ROBOT_STOP, 0);
}

RobotState fsm_get_state(void)
{
    return current_state;
}

void fsm_update(RobotEvent event)
{
    unsigned long now = get_millis();
    unsigned long elapsed = now - state_entry_time;

    /*
     * Solo se inicia una nueva maniobra de evasión
     * cuando el robot está avanzando.
     *
     * Durante el retroceso y el giro se ignora
     * temporalmente el mismo obstáculo.
     */
    if (current_state == STATE_MOVING_FORWARD) {
        if (sensor_obstacle_detected() ||
            sensor_side_obstacle_detected()) {

            event = EVENT_OBSTACLE;
        }
    }

    /*
     * EVENT_STOP tiene prioridad en cualquier estado.
     */
    if (event == EVENT_STOP) {
        current_state = STATE_STOPPED;
        state_entry_time = now;

        robot_move(ROBOT_STOP, 0);
        return;
    }

    switch (current_state) {

        case STATE_IDLE:
            if (event == EVENT_START) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_MOVING_FORWARD:
            if (event == EVENT_OBSTACLE) {
                current_state = STATE_AVOID_STOP;
                state_entry_time = now;

                robot_move(ROBOT_STOP, 0);
            }
            break;

        case STATE_AVOID_STOP:
            if (elapsed >= STOP_TIME_MS) {
                current_state = STATE_BACKING_UP;
                state_entry_time = now;

                robot_move(ROBOT_BACKWARD, MOTOR_SPEED);
            }
            break;

        case STATE_BACKING_UP:
            if (elapsed >= BACKWARD_TIME_MS) {
                int turn_direction = choose_turn_direction();

                turn_duration =
                    TURN_MIN_TIME_MS +
                    (rand() % TURN_RANDOM_MS);

                state_entry_time = now;

                if (turn_direction < 0) {
                    current_state = STATE_TURNING_LEFT;
                    robot_move(ROBOT_TURN_LEFT, MOTOR_SPEED);
                }
                else {
                    current_state = STATE_TURNING_RIGHT;
                    robot_move(ROBOT_TURN_RIGHT, MOTOR_SPEED);
                }
            }
            break;

        case STATE_TURNING_LEFT:
            if (elapsed >= turn_duration) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_TURNING_RIGHT:
            if (elapsed >= turn_duration) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_STOPPED:
            robot_move(ROBOT_STOP, 0);
            break;

        case STATE_EMERGENCY:
            robot_move(ROBOT_STOP, 0);
            break;

        default:
            robot_move(ROBOT_STOP, 0);
            break;
    }
}