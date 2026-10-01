#include <stdio.h>
#include <unistd.h>
#include "motors.h"
#include "sensors.h"
#include "fsm.h"

int main(void) {
    if (motors_init() != 0 || sensors_init() != 0) {
        return 1;
    }

    fsm_init();
    sleep(3); // Pausa de 3 segundos de seguridad al encender
    fsm_update(EVENT_START);

    // Bucle de navegación autónoma durante 60 segundos (6000 ciclos de 10 ms)
    for (int i = 0; i < 6000; i++) {
        fsm_update(EVENT_NONE);
        usleep(10000); // 10 ms por ciclo
    }

    fsm_update(EVENT_STOP);
    motors_cleanup();
    return 0;
}