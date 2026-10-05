#include <stdio.h>
#include <unistd.h>

#include "motors.h"
#include "sensors.h"
#include "fsm.h"
#include "leds.h"

int main(void)
{
    /*
     * Inicializar los LEDs primero para poder indicar
     * que el sistema se encuentra activo.
     */
    if (leds_init() != 0) {
        return 1;
    }

    led_system_set(1);
    led_autonomous_set(0);
    led_manual_set(0);
    led_obstacle_set(0);

    /*
     * Inicializar motores y sensores.
     */
    if (motors_init() != 0 || sensors_init() != 0) {
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    fsm_init();

    /*
     * Pequeña espera de seguridad antes de comenzar
     * el movimiento autónomo.
     */
    sleep(3);

    /*
     * El robot entra en modo autónomo.
     */
    led_autonomous_set(1);
    led_manual_set(0);

    fsm_update(EVENT_START);

    /*
     * Ejecutar navegación autónoma durante aproximadamente
     * 60 segundos. La FSM se actualiza cada 10 ms.
     */
    for (int i = 0; i < 6000; i++) {
        fsm_update(EVENT_NONE);
        usleep(10000);
    }

    /*
     * Detener el robot y apagar indicadores.
     */
    fsm_update(EVENT_STOP);

    led_obstacle_set(0);
    led_autonomous_set(0);
    led_manual_set(0);
    led_system_set(0);

    motors_cleanup();
    leds_cleanup();

    return 0;
}
