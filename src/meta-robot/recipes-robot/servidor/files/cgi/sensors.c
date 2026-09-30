#include <stdio.h>
#include <stdlib.h>
#include "librobot.h"
#include "session.h"

int main(void) {
    char *auth = getenv("HTTP_AUTHORIZATION");
    if (!auth || !session_validate(auth)) {
        printf("Status: 401\r\n");
        printf("Content-Type: application/json\r\n\r\n");
        printf("{\"status\":\"error\",\"message\":\"Token inválido o expirado\"}");
        return 0;
    }

    float front = robot_sensor_read_distance(SENSOR_FRONT);
    float left  = robot_sensor_read_distance(SENSOR_LEFT);
    float right = robot_sensor_read_distance(SENSOR_RIGHT);

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\",\"data\":{\"front\":%.2f,\"left\":%.2f,\"right\":%.2f}}",
           front, left, right);
    return 0;
}