#include <stdio.h>
#include <unistd.h>
#include "motors.h"
#include "sensors.h"
#include "fsm.h"

int main(void) {
    setbuf(stdout, NULL);
    printf("=== NAVEGACIÓN AUTÓNOMA CON EVASIÓN FSM ===\n");

    if (motors_init() != 0 || sensors_init() != 0) {
        printf("[ERROR] Fallo al inicializar los periféricos GPIO.\n");
        return 1;
    }

    fsm_init();

    // Pausa inicial de seguridad de 3 segundos
    sleep(3);
    fsm_update(EVENT_START);

    // Bucle de control autónomo durante 45 segundos (4500 ciclos a 10 ms)
    for (int i = 0; i < 4500; i++) {
        fsm_update(EVENT_NONE);
        usleep(10000); // 10 ms por ciclo
    }

    // Apagar la FSM y limpiar los pines GPIO
    fsm_update(EVENT_STOP);
    motors_cleanup();
    printf("[MAIN] Prueba autónoma finalizada.\n");

    return 0;
}