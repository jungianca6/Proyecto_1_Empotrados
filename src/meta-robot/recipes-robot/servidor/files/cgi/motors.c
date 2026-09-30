#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"
#include "librobot.h"

static void send_error(int http_code, const char *msg) {
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

int main(void) {
    // TODO: reemplazar por validación real de sesión cuando esté listo /cgi-bin/login
    // char *auth = getenv("HTTP_AUTHORIZATION");
    // if (!auth || !session_validate(auth)) { send_error(401, "Token inválido o expirado"); return 0; }

    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;

    if (len <= 0 || len >= 1024) {
        send_error(400, "Body inválido o ausente");
        return 0;
    }

    char body[1024] = {0};
    fread(body, 1, len, stdin);

    cJSON *json = cJSON_Parse(body);
    if (!json) {
        send_error(400, "JSON inválido");
        return 0;
    }

    cJSON *motor = cJSON_GetObjectItem(json, "motor");
    cJSON *speed = cJSON_GetObjectItem(json, "speed");

    if (!cJSON_IsString(motor) || !cJSON_IsNumber(speed)) {
        cJSON_Delete(json);
        send_error(400, "Faltan campos 'motor' o 'speed'");
        return 0;
    }

    motor_id_t id;
    if (strcmp(motor->valuestring, "left") == 0) {
        id = MOTOR_LEFT;
    } else if (strcmp(motor->valuestring, "right") == 0) {
        id = MOTOR_RIGHT;
    } else {
        cJSON_Delete(json);
        send_error(400, "Valor de 'motor' debe ser 'left' o 'right'");
        return 0;
    }

    int result = robot_motor_set(id, speed->valueint);
    cJSON_Delete(json);

    if (result != 0) {
        send_error(500, "Fallo al mover el motor");
        return 0;
    }

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\"}");
    return 0;
}