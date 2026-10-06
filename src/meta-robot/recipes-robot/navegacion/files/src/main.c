#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "motors.h"
#include "sensors.h"
#include "fsm.h"
#include "leds.h"
#include "brushes.h"

static volatile sig_atomic_t running = 1;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int main(void)
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    /*
     * Inicializar LEDs.
     */
    if (leds_init() != 0) {
        return 1;
    }

    led_system_set(1);
    led_autonomous_set(0);
    led_manual_set(0);
    led_obstacle_set(0);

    /*
     * Inicializar cepillos.
     */
    if (brushes_init() != 0) {
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    /*
     * Inicializar motores de movimiento y sensores.
     */
    if (motors_init() != 0 || sensors_init() != 0) {
        brushes_cleanup();
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    fsm_init();

    /*
     * Espera de seguridad antes de iniciar movimiento.
     */
    sleep(3);

    /*
     * Activar modo autónomo.
     */
    led_autonomous_set(1);
    led_manual_set(0);

    /*
     * Encender los dos cepillos.
     */
    brushes_on();

    /*
     * Iniciar navegación autónoma.
     */
    fsm_update(EVENT_START);

    while (running) {
        fsm_update(EVENT_NONE);
        usleep(10000);
    }

    /*
     * Apagado seguro.
     */
    fsm_update(EVENT_STOP);

    brushes_off();

    led_obstacle_set(0);
    led_autonomous_set(0);
    led_manual_set(0);
    led_system_set(0);

    motors_cleanup();
    brushes_cleanup();
    leds_cleanup();

    return 0;
}
