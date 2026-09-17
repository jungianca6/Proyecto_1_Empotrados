#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "motors.h"

int main(void) {
    setbuf(stdout, NULL);
    printf("=== PRUEBA DE SECUENCIA COMPLETA ===\n");

    if (motors_init() != 0) {
        printf("[ERROR] Fallo al inicializar los pines GPIO.\n");
        return 1;
    }

    printf("Pausa inicial de 5 segundos...\n");
    sleep(5);

    printf("1. Avanzando en linea recta (3s)...\n");
    robot_move(ROBOT_FORWARD, 100);
    sleep(3);

    printf("2. Parada intermedia (1s)...\n");
    robot_move(ROBOT_STOP, 0);
    sleep(1);

    printf("3. Retrocediendo (3s)...\n");
    robot_move(ROBOT_BACKWARD, 100);
    sleep(3);

    printf("4. Parada intermedia (1s)...\n");
    robot_move(ROBOT_STOP, 0);
    sleep(1);

    printf("5. GIRO IZQUIERDA (2s)...\n");
    robot_move(ROBOT_TURN_LEFT, 100);
    sleep(2);

    printf("6. GIRO DERECHA (2s)...\n");
    robot_move(ROBOT_TURN_RIGHT, 100);
    sleep(2);

    printf("7. Apagando motores...\n");
    motors_cleanup();
    printf("Secuencia finalizada.\n");

    return 0;
}