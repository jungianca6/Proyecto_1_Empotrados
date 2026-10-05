#ifndef LIBROBOT_H
#define LIBROBOT_H

/*
 * API pública de librobot.
 *
 * Esta interfaz es utilizada por las aplicaciones que necesitan
 * acceder al hardware del robot, por ejemplo:
 *
 *   - servidor CGI
 *   - aplicación de navegación
 *
 * La implementación interna se encuentra en motors.c, sensors.c,
 * gpio.c, etc.
 */

#include "motors.h"
#include "sensors.h"


/* ============================================================
 * MOTORES
 * ============================================================
 *
 * La implementación actual de motors.c trabaja mediante
 * movimientos del robot completo, no mediante control
 * independiente de cada motor.
 */

int robot_motor_init(void);

int robot_motor_deinit(void);

/*
 * Ejecuta un movimiento del robot.
 *
 * speed_pct:
 *   0   -> detenido
 *   1-100 -> velocidad solicitada
 *
 * Nota:
 * La implementación actual de motors.c todavía no realiza
 * PWM real; speed_pct se utiliza como parámetro de la API,
 * pero el código actual activa los motores a plena potencia.
 */
int robot_motor_move(RobotDirection direction, int speed_pct);

/*
 * Detiene ambos motores.
 */
int robot_motor_stop_all(void);


/* ============================================================
 * SENSORES
 * ============================================================
 *
 * Los sensores actuales son FC-51 digitales.
 *
 * No proporcionan distancia en centímetros. Solamente indican
 * si existe o no un obstáculo.
 */

int robot_sensor_init(void);

int robot_sensor_deinit(void);

/*
 * Retorna:
 *   1 -> obstáculo detectado
 *   0 -> despejado
 *  -1 -> error
 */
int robot_sensor_front_obstacle(void);

int robot_sensor_side_obstacle(void);


/* ============================================================
 * LEDs
 * ============================================================
 *
 * API reservada para la implementación de los cuatro LEDs
 * del proyecto.
 *
 * Actualmente estos módulos todavía no forman parte del código
 * de navegación mostrado.
 */

typedef enum {
    LED_AUTO = 0,
    LED_MANUAL,
    LED_OBSTACLE,
    LED_POWER
} robot_led_t;

int robot_led_init(void);

int robot_led_deinit(void);

int robot_led_set(robot_led_t led, int state);


/* ============================================================
 * AUDIO
 * ============================================================
 *
 * API pública para el módulo de audio.
 */

int robot_audio_init(void);

int robot_audio_deinit(void);

int robot_audio_play(const char *filepath);

int robot_audio_pause(void);

int robot_audio_stop(void);

int robot_audio_set_volume(int volume);


/* ============================================================
 * ESTADO DE NAVEGACIÓN
 * ============================================================
 *
 * La FSM continúa siendo parte del código de navegación
 * desarrollado actualmente.
 *
 * Estos métodos permiten que otros componentes consulten
 * el estado si posteriormente se decide exponerlo mediante
 * el servidor.
 */

#include "fsm.h"

int robot_navigation_state(void);

#endif /* LIBROBOT_H */