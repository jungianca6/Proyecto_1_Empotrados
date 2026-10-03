#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librobot.h"
#include "session.h"

#define MODE_FILE "/tmp/robot_mode"

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

static void read_mode(char *out, size_t out_len)
{
    FILE *f = fopen(MODE_FILE, "r");

    if (!f) {
        snprintf(out, out_len, "autonomous");
        return;
    }

    if (fscanf(f, "%63s", out) != 1)
        snprintf(out, out_len, "autonomous");

    fclose(f);
}

int main(void)
{
    char token[128];

    if (get_token(token, sizeof(token)) != 0 ||
        !session_validate(token)) {

        send_error(401, "Token invalido o expirado");
        return 0;
    }

    char mode[64];

    read_mode(mode, sizeof(mode));

    int front = robot_sensor_front_obstacle();
    int side = robot_sensor_side_obstacle();

    if (front < 0 || side < 0) {
        send_error(
            500,
            "Error leyendo los sensores"
        );

        return 0;
    }

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"mode\":\"%s\","
            "\"sensors\":{"
                "\"front_obstacle\":%d,"
                "\"side_obstacle\":%d"
            "},"
            "\"leds\":{"
                "\"implemented\":false"
            "},"
            "\"audio\":{"
                "\"implemented\":false"
            "}"
        "}"
        "}",
        mode,
        front,
        side
    );

    return 0;
}