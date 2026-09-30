#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "session.h"

// Usuario de prueba fijo. TODO: mover a archivo de configuración o hash real.
#define VALID_USER "admin"
#define VALID_PASS "robot123"

static void send_error(int http_code, const char *msg) {
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

int main(void) {
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

    cJSON *user = cJSON_GetObjectItem(json, "username");
    cJSON *pass = cJSON_GetObjectItem(json, "password");

    if (!cJSON_IsString(user) || !cJSON_IsString(pass)) {
        cJSON_Delete(json);
        send_error(400, "Faltan campos 'username' o 'password'");
        return 0;
    }

    if (strcmp(user->valuestring, VALID_USER) != 0 ||
        strcmp(pass->valuestring, VALID_PASS) != 0) {
        cJSON_Delete(json);
        send_error(401, "Usuario o contraseña incorrectos");
        return 0;
    }

    char token[65];
    if (session_create(user->valuestring, token, sizeof(token)) != 0) {
        cJSON_Delete(json);
        send_error(500, "No se pudo crear la sesión");
        return 0;
    }

    cJSON_Delete(json);

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\",\"token\":\"%s\"}", token);
    return 0;
}