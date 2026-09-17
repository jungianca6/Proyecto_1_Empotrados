#include <stdio.h>
#include <unistd.h>
#include "motors.h"
#include "fsm.h"

int main(void) {
    setbuf(stdout, NULL);
    printf("=== CONTROL DE ROBOT POR MÁQUINA DE ESTADOS ===\n");

    if (motors_init() != 0) {
        printf("[ERROR] No se pudieron inicializar los motores.\n");
        return 1;
    }

    fsm_init();

    // Arrancar la secuencia tras 2 segundos de espera
    sleep(2);
    fsm_update(EVENT_START);

    // Bucle principal de control a 100 Hz (10 ms por ciclo)
    while (fsm_get_state() != STATE_STOPPED) {
        fsm_update(EVENT_NONE); // Actualización periódica no bloqueante
        usleep(10000);          // 10,000 us = 10 ms
    }

    motors_cleanup();
    printf("[MAIN] Programa finalizado correctamente.\n");
    return 0;
}