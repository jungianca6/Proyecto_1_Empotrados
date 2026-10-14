#include "librobot.h"
#include "brushes.h"
#include "vacuum.h"
#include "leds.h"

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
 */

int robot_led_init(void)
{
    return leds_init();
}

int robot_led_deinit(void)
{
    leds_cleanup();
    return 0;
}

int robot_led_set(robot_led_t led, int state)
{
    switch (led) {
        case LED_AUTO:
            led_autonomous_set(state);
            break;

        case LED_MANUAL:
            led_manual_set(state);
            break;

        case LED_OBSTACLE:
            led_obstacle_set(state);
            break;

        case LED_POWER:
            led_system_set(state);
            break;

        default:
            return -1;
    }

    return 0;
}

int robot_led_get(robot_led_t led)
{
    switch (led) {
        case LED_AUTO:
            return led_autonomous_get();

        case LED_MANUAL:
            return led_manual_get();

        case LED_OBSTACLE:
            return led_obstacle_get();

        case LED_POWER:
            return led_system_get();

        default:
            return -1;
    }
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
 * CEPILLOS
 * ============================================================ */

int robot_brushes_on(void)
{
    brushes_on();
    return 0;
}

int robot_brushes_off(void)
{
    brushes_off();
    return 0;
}

/* ============================================================
 * MOTOR DE ASPIRACION
 * ============================================================ */

int robot_vacuum_on(void)
{
    vacuum_on();
    return 0;
}

int robot_vacuum_off(void)
{
    vacuum_off();
    return 0;
}

/* ============================================================
 * NAVEGACIÓN / FSM
 * ============================================================
 */

int robot_navigation_state(void)
{
    return (int)fsm_get_state();
}