#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "session.h"

#define METRICS_FILE "/var/lib/robot/metrics.json"

static void send_error(int http_code, const char *message)
{
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"error\","
        "\"message\":\"%s\""
        "}\n",
        message
    );
}

static int get_token(char *token, size_t token_size)
{
    char *auth = getenv("HTTP_AUTHORIZATION");

    if (!auth || auth[0] == '\0')
        return -1;

    /*
     * Compatible tanto con:
     *
     * Authorization: TOKEN
     *
     * como:
     *
     * Authorization: Bearer TOKEN
     */
    if (strncmp(auth, "Bearer ", 7) == 0)
        auth += 7;

    if (strlen(auth) >= token_size)
        return -1;

    strcpy(token, auth);

    return 0;
}

int main(void)
{
    char token[128];

    if (get_token(token, sizeof(token)) != 0 ||
        !session_validate(token)) {

        send_error(
            401,
            "Token invalido o expirado"
        );

        return 0;
    }

    FILE *file = fopen(METRICS_FILE, "r");

    if (!file) {
        send_error(
            503,
            "Metricas aun no disponibles"
        );

        return 0;
    }

    printf("Content-Type: application/json\r\n");
    printf("Cache-Control: no-store\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":"
    );

    char buffer[512];
    size_t bytes_read;

    while ((bytes_read =
                fread(buffer, 1, sizeof(buffer), file)) > 0) {

        fwrite(
            buffer,
            1,
            bytes_read,
            stdout
        );
    }

    fclose(file);

    printf("}\n");

    return 0;
}