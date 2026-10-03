#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "session.h"

#define MODE_FILE "/tmp/robot_mode"

static void send_error(int http_code, const char *msg) {
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

static int write_mode(const char *mode) {
    FILE *f = fopen(MODE_FILE, "w");
    if (!f) return -1;
    fprintf(f, "%s", mode);
    fclose(f);
    return 0;
}

static void read_mode(char *out, int out_len) {
    FILE *f = fopen(MODE_FILE, "r");
    if (!f) {
        snprintf(out, out_len, "autonomous"); // default
        return;
    }
    if (fscanf(f, "%63s", out) != 1) {
        snprintf(out, out_len, "autonomous");
    }
    fclose(f);
}

int main(void) {
    char *auth = getenv("HTTP_AUTHORIZATION");
    if (!auth || !session_validate(auth)) {
        send_error(401, "Token inválido o expirado");
        return 0;
    }

    char *method = getenv("REQUEST_METHOD");

    if (method && strcmp(method, "GET") == 0) {
        char mode[64];
        read_mode(mode, sizeof(mode));
        printf("Content-Type: application/json\r\n\r\n");
        printf("{\"status\":\"ok\",\"data\":{\"mode\":\"%s\"}}", mode);
        return 0;
    }

    // POST: cambiar modo
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

    cJSON *mode = cJSON_GetObjectItem(json, "mode");
    if (!cJSON_IsString(mode) ||
        (strcmp(mode->valuestring, "autonomous") != 0 &&
         strcmp(mode->valuestring, "manual") != 0)) {
        cJSON_Delete(json);
        send_error(400, "Valor de 'mode' debe ser 'autonomous' o 'manual'");
        return 0;
    }

    int result = write_mode(mode->valuestring);
    cJSON_Delete(json);

    if (result != 0) {
        send_error(500, "No se pudo guardar el modo");
        return 0;
    }

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\"}");
    return 0;
}