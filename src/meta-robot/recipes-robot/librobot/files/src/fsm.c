#include <stdlib.h>
#include <time.h>

#include "fsm.h"
#include "sensors.h"
#include "leds.h"

/*
 * Algoritmo de cobertura reactiva con evasión de obstáculos
 * ---------------------------------------------------------
 *
 * El robot avanza mientras el camino esté despejado.
 *
 * Cuando detecta un obstáculo:
 * 1. Se detiene inmediatamente.
 * 2. Enciende el LED de obstáculo.
 * 3. Retrocede para alejarse.
 * 4. Decide el sentido del giro según los sensores.
 * 5. Gira durante un intervalo variable.
 * 6. Continúa avanzando y apaga el LED de obstáculo.
 *
 * Sensores:
 * - Sensor frontal: detecta obstáculos frente al robot.
 * - Sensor lateral: está ubicado en el lado izquierdo.
 *
 * Estrategia de evasión:
 * - Obstáculo frontal con izquierda libre -> girar izquierda.
 * - Obstáculo frontal con izquierda ocupada -> girar derecha.
 * - Obstáculo solamente a la izquierda -> girar derecha.
 *
 * La duración variable del giro ayuda a evitar trayectorias
 * demasiado repetitivas durante la cobertura.
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
 * Dirección planificada para la maniobra:
 * -1 = izquierda
 *  1 = derecha
 */
static int planned_turn = 1;

static unsigned long get_millis(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000UL) +
           (ts.tv_nsec / 1000000UL);
}

/*
 * Determina hacia dónde debe girar el robot según
 * la combinación de los sensores.
 *
 * El sensor lateral se encuentra en el lado izquierdo.
 */
static int choose_turn_direction(int front_obstacle, int left_obstacle)
{
    /*
     * Si la izquierda está bloqueada, el robot
     * debe alejarse girando hacia la derecha.
     */
    if (left_obstacle) {
        return 1;
    }

    /*
     * Si solamente existe un obstáculo frontal
     * y la izquierda está libre, gira a la izquierda.
     */
    if (front_obstacle) {
        return -1;
    }

    /*
     * Caso de respaldo. Normalmente no debería alcanzarse.
     */
    return (rand() % 2) ? 1 : -1;
}

void fsm_init(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)ts.tv_nsec);

    current_state = STATE_IDLE;
    state_entry_time = get_millis();

    led_obstacle_set(0);

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
     * Los sensores se revisan solamente mientras el robot
     * avanza. Así evitamos que el mismo obstáculo vuelva
     * a disparar eventos durante el retroceso o el giro.
     */
    if (current_state == STATE_MOVING_FORWARD) {

        int front_obstacle = sensor_obstacle_detected();
        int left_obstacle = sensor_side_obstacle_detected();

        if (front_obstacle || left_obstacle) {

            planned_turn =
                choose_turn_direction(front_obstacle,
                                      left_obstacle);

            event = EVENT_OBSTACLE;
        }
    }

    /*
     * EVENT_STOP tiene prioridad en cualquier estado.
     */
    if (event == EVENT_STOP) {
        current_state = STATE_STOPPED;
        state_entry_time = now;

        led_obstacle_set(0);

        robot_move(ROBOT_STOP, 0);
        return;
    }

    switch (current_state) {

        case STATE_IDLE:
            if (event == EVENT_START) {
                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                led_obstacle_set(0);

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_MOVING_FORWARD:
            if (event == EVENT_OBSTACLE) {

                /*
                 * Se encontró un obstáculo.
                 * Detener inmediatamente y activar indicador.
                 */
                current_state = STATE_AVOID_STOP;
                state_entry_time = now;

                led_obstacle_set(1);

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

                /*
                 * La duración del giro cambia ligeramente
                 * para reducir trayectorias repetitivas.
                 */
                turn_duration =
                    TURN_MIN_TIME_MS +
                    (rand() % TURN_RANDOM_MS);

                state_entry_time = now;

                if (planned_turn < 0) {
                    current_state = STATE_TURNING_LEFT;

                    robot_move(ROBOT_TURN_LEFT,
                               MOTOR_SPEED);
                }
                else {
                    current_state = STATE_TURNING_RIGHT;

                    robot_move(ROBOT_TURN_RIGHT,
                               MOTOR_SPEED);
                }
            }
            break;

        case STATE_TURNING_LEFT:
            if (elapsed >= turn_duration) {

                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                led_obstacle_set(0);

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_TURNING_RIGHT:
            if (elapsed >= turn_duration) {

                current_state = STATE_MOVING_FORWARD;
                state_entry_time = now;

                led_obstacle_set(0);

                robot_move(ROBOT_FORWARD, MOTOR_SPEED);
            }
            break;

        case STATE_STOPPED:
            led_obstacle_set(0);
            robot_move(ROBOT_STOP, 0);
            break;

        case STATE_EMERGENCY:
            led_obstacle_set(1);
            robot_move(ROBOT_STOP, 0);
            break;

        default:
            led_obstacle_set(0);
            robot_move(ROBOT_STOP, 0);
            break;
    }
}
