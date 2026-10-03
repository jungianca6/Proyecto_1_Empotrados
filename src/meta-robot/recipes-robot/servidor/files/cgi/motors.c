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

    /*
     * Se acepta:
     *
     * Authorization: <token>
     *
     * y:
     *
     * Authorization: Bearer <token>
     */

    if (strncmp(auth, "Bearer ", 7) == 0)
        auth += 7;

    if (strlen(auth) >= token_size)
        return -1;

    strcpy(token, auth);

    return 0;
}

static RobotDirection parse_direction(const char *direction)
{
    if (strcmp(direction, "forward") == 0)
        return ROBOT_FORWARD;

    if (strcmp(direction, "backward") == 0)
        return ROBOT_BACKWARD;

    if (strcmp(direction, "left") == 0)
        return ROBOT_TURN_LEFT;

    if (strcmp(direction, "right") == 0)
        return ROBOT_TURN_RIGHT;

    return ROBOT_STOP;
}

int main(void)
{
    char token[128];

    if (get_token(token, sizeof(token)) != 0 ||
        !session_validate(token)) {

        send_error(401, "Token invalido o expirado");
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

    cJSON *direction = cJSON_GetObjectItem(json, "direction");
    cJSON *speed = cJSON_GetObjectItem(json, "speed");

    if (!cJSON_IsString(direction) ||
        !cJSON_IsNumber(speed)) {

        cJSON_Delete(json);

        send_error(
            400,
            "Faltan campos direction o speed"
        );

        return 0;
    }

    int speed_pct = speed->valueint;

    if (speed_pct < 0 || speed_pct > 100) {
        cJSON_Delete(json);

        send_error(
            400,
            "speed debe estar entre 0 y 100"
        );

        return 0;
    }

    RobotDirection dir =
        parse_direction(direction->valuestring);

    if (dir == ROBOT_STOP &&
        strcmp(direction->valuestring, "stop") != 0) {

        cJSON_Delete(json);

        send_error(
            400,
            "Direccion invalida"
        );

        return 0;
    }

    int result;

    if (strcmp(direction->valuestring, "stop") == 0) {
        result = robot_motor_stop_all();
    } else {
        result = robot_motor_move(dir, speed_pct);
    }

    cJSON_Delete(json);

    if (result != 0) {
        send_error(
            500,
            "Fallo al controlar los motores"
        );

        return 0;
    }

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{\"status\":\"ok\",\"data\":{\"direction\":\"%s\",\"speed\":%d}}",
        direction->valuestring,
        speed_pct
    );

    return 0;
}