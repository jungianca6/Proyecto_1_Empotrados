#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "motors.h"
#include "sensors.h"
#include "fsm.h"
#include "leds.h"

/*
 * Bandera global utilizada para terminar el programa
 * de forma segura cuando se recibe SIGINT o SIGTERM.
 */
static volatile sig_atomic_t running = 1;

/*
 * Manejador de señales.
 *
 * No se realizan operaciones de hardware aquí;
 * únicamente se cambia la bandera para que el
 * bucle principal pueda finalizar ordenadamente.
 */
static void handle_signal(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int main(void)
{
    /*
     * Permitir que el programa pueda detenerse
     * correctamente desde el sistema.
     */
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
     * Inicializar motores y sensores.
     */
    if (motors_init() != 0 || sensors_init() != 0) {
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    fsm_init();

    /*
     * Espera de seguridad antes del movimiento.
     */
    sleep(3);

    /*
     * Activar modo autónomo.
     */
    led_autonomous_set(1);
    led_manual_set(0);

    fsm_update(EVENT_START);

    /*
     * Navegación autónoma continua.
     *
     * La FSM se actualiza cada 10 ms.
     */
    while (running) {
        fsm_update(EVENT_NONE);
        usleep(10000);
    }

    /*
     * Apagado seguro.
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
