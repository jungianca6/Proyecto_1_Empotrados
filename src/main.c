#include <stdio.h>
#include <unistd.h>
#include "motors.h"
#include "sensors.h"

int main(void) {
    if (motors_init() != 0 || sensors_init() != 0) {
        return 1;
    }

    // Pausa inicial de 3 segundos antes de iniciar el monitoreo
    sleep(3);

    // Monitoreo en vivo durante 30 segundos
    for (int i = 0; i < 600; i++) {
        if (sensor_obstacle_detected()) {
            // Si la mano está frente al sensor -> Girar motores
            robot_move(ROBOT_FORWARD, 100);
        } else {
            // Si la vía está libre -> Detener motores
            robot_move(ROBOT_STOP, 0);
        }
        usleep(50000); // Muestreo rápido cada 50 ms
    }

    motors_cleanup();
    return 0;
}