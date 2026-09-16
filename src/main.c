#include <stdio.h>
#include <unistd.h>
#include "motors.h"

int main() {
    setbuf(stdout, NULL);
    printf("=== Prueba Extendida de Motores ===\n");

    if (motors_init() != 0) {
        printf("Error al inicializar controladores de motores.\n");
        return 1;
    }

    printf("1. Avanzando al 90%% de potencia durante 5 segundos...\n");
    robot_move(ROBOT_FORWARD, 90);
    sleep(5);

    printf("2. Giro a la izquierda al 80%% durante 3 segundos...\n");
    robot_move(ROBOT_TURN_LEFT, 80);
    sleep(3);

    printf("3. Avanzando de nuevo al 90%% durante 5 segundos...\n");
    robot_move(ROBOT_FORWARD, 90);
    sleep(5);

    printf("4. Deteniendo robot...\n");
    robot_move(ROBOT_STOP, 0);

    motors_cleanup();
    return 0;
}