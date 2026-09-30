#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "librobot.h"
#include "session.h"

static void send_error(int http_code, const char *msg) {
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

int main(void) {
    char *auth = getenv("HTTP_AUTHORIZATION");
    if (!auth || !session_validate(auth)) {
        send_error(401, "Token inválido o expirado");
        return 0;
    }

    char *method = getenv("REQUEST_METHOD");

    if (method && strcmp(method, "GET") == 0) {
        // Consultar estado (nota: el stub no guarda estado real, esto asume
        // que robot_led_get() existiría; por ahora reportamos apagado fijo
        // hasta que el módulo real de LEDs esté implementado en).
        printf("Content-Type: application/json\r\n\r\n");
        printf("{\"status\":\"ok\",\"data\":{\"auto\":0,\"manual\":0,\"obstacle\":0,\"power\":1}}");
        return 0;
    }

    // POST: cambiar estado de un LED
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

    cJSON *led = cJSON_GetObjectItem(json, "led");
    cJSON *state = cJSON_GetObjectItem(json, "state");

    if (!cJSON_IsString(led) || !cJSON_IsNumber(state)) {
        cJSON_Delete(json);
        send_error(400, "Faltan campos 'led' o 'state'");
        return 0;
    }

    led_id_t id;
    if (strcmp(led->valuestring, "auto") == 0) id = LED_AUTO;
    else if (strcmp(led->valuestring, "manual") == 0) id = LED_MANUAL;
    else if (strcmp(led->valuestring, "obstacle") == 0) id = LED_OBSTACLE;
    else if (strcmp(led->valuestring, "power") == 0) id = LED_POWER;
    else {
        cJSON_Delete(json);
        send_error(400, "Valor de 'led' inválido");
        return 0;
    }

    int result = robot_led_set(id, state->valueint);
    cJSON_Delete(json);

    if (result != 0) {
        send_error(500, "Fallo al cambiar el LED");
        return 0;
    }

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\"}");
    return 0;
}