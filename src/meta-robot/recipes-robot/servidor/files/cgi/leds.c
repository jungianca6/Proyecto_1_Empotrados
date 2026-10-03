#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "librobot.h"
#include "session.h"

static void send_error(int http_code, const char *msg)
{
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{\"status\":\"error\",\"message\":\"%s\"}",
        msg
    );
}

static int get_token(char *token, size_t token_size)
{
    char *auth = getenv("HTTP_AUTHORIZATION");

    if (!auth || auth[0] == '\0')
        return -1;

    if (strncmp(auth, "Bearer ", 7) == 0)
        auth += 7;

    if (strlen(auth) >= token_size)
        return -1;

    strcpy(token, auth);

    return 0;
}

static int parse_led(const char *name, robot_led_t *led)
{
    if (strcmp(name, "auto") == 0) {
        *led = LED_AUTO;
        return 0;
    }

    if (strcmp(name, "manual") == 0) {
        *led = LED_MANUAL;
        return 0;
    }

    if (strcmp(name, "obstacle") == 0) {
        *led = LED_OBSTACLE;
        return 0;
    }

    if (strcmp(name, "power") == 0) {
        *led = LED_POWER;
        return 0;
    }

    return -1;
}

int main(void)
{
    char token[128];

    if (get_token(token, sizeof(token)) != 0 ||
        !session_validate(token)) {

        send_error(401, "Token invalido o expirado");
        return 0;
    }

    char *method = getenv("REQUEST_METHOD");

    if (method &&
        strcmp(method, "GET") == 0) {

        printf("Content-Type: application/json\r\n");
        printf("\r\n");

        printf(
            "{"
            "\"status\":\"ok\","
            "\"data\":{"
                "\"implemented\":false,"
                "\"message\":\"Modulo de LEDs pendiente\""
            "}"
            "}"
        );

        return 0;
    }

    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;

    if (len <= 0 || len >= 1024) {
        send_error(400, "Body invalido o ausente");
        return 0;
    }

    char body[1024] = {0};

    if (fread(body, 1, (size_t)len, stdin) != (size_t)len) {
        send_error(400, "No se pudo leer el body");
        return 0;
    }

    cJSON *json = cJSON_Parse(body);

    if (!json) {
        send_error(400, "JSON invalido");
        return 0;
    }

    cJSON *led_json =
        cJSON_GetObjectItem(json, "led");

    cJSON *state_json =
        cJSON_GetObjectItem(json, "state");

    if (!cJSON_IsString(led_json) ||
        !cJSON_IsNumber(state_json)) {

        cJSON_Delete(json);

        send_error(
            400,
            "Faltan campos led o state"
        );

        return 0;
    }

    robot_led_t led;

    if (parse_led(led_json->valuestring, &led) != 0) {
        cJSON_Delete(json);

        send_error(
            400,
            "LED invalido"
        );

        return 0;
    }

    int state = state_json->valueint;

    if (state != 0 && state != 1) {
        cJSON_Delete(json);

        send_error(
            400,
            "state debe ser 0 o 1"
        );

        return 0;
    }

    int result = robot_led_set(led, state);

    cJSON_Delete(json);

    if (result != 0) {
        send_error(
            501,
            "Modulo de LEDs no implementado"
        );

        return 0;
    }

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{\"status\":\"ok\"}"
    );

    return 0;
}