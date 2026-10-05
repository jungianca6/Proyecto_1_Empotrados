#include "librobot.h"


/* ============================================================
 * MOTORES
 * ============================================================
 */

int robot_motor_init(void)
{
    return motors_init();
}

int robot_motor_deinit(void)
{
    motors_cleanup();
    return 0;
}

int robot_motor_move(RobotDirection direction, int speed_pct)
{
    if (speed_pct < 0) {
        speed_pct = 0;
    }

    if (speed_pct > 100) {
        speed_pct = 100;
    }

    robot_move(direction, speed_pct);

    return 0;
}

int robot_motor_stop_all(void)
{
    robot_move(ROBOT_STOP, 0);

    return 0;
}


/* ============================================================
 * SENSORES
 * ============================================================
 */

int robot_sensor_init(void)
{
    return sensors_init();
}

int robot_sensor_deinit(void)
{
    /*
     * La implementación actual de sensors.c no tiene una
     * función sensors_cleanup().
     *
     * Por ahora no hay recursos adicionales que liberar
     * desde esta capa.
     */
    return 0;
}

int robot_sensor_front_obstacle(void)
{
    return sensor_obstacle_detected();
}

int robot_sensor_side_obstacle(void)
{
    return sensor_side_obstacle_detected();
}


/* ============================================================
 * LEDs
 * ============================================================
 *
 * Todavía no implementados.
 */

int robot_led_init(void)
{
    return -1;
}

int robot_led_deinit(void)
{
    return 0;
}

int robot_led_set(robot_led_t led, int state)
{
    (void)led;
    (void)state;

    return -1;
}


/* ============================================================
 * AUDIO
 * ============================================================
 *
 * Todavía no implementado.
 */

int robot_audio_init(void)
{
    return -1;
}

int robot_audio_deinit(void)
{
    return 0;
}

int robot_audio_play(const char *filepath)
{
    (void)filepath;

    return -1;
}

int robot_audio_pause(void)
{
    return -1;
}

int robot_audio_stop(void)
{
    return -1;
}

int robot_audio_set_volume(int volume)
{
    (void)volume;

    return -1;
}


/* ============================================================
 * NAVEGACIÓN / FSM
 * ============================================================
 */

int robot_navigation_state(void)
{
    return (int)fsm_get_state();
}