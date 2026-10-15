#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "librobot.h"
#include "session.h"

#define MAX_BODY_SIZE 256

static void respond(int status, const char *reason, const char *json)
{
    printf("Status: %d %s\r\n", status, reason);
    printf("Content-Type: application/json\r\n");
    printf("Cache-Control: no-store\r\n\r\n");
    printf("%s\n", json);
}


static int authorized(void)
{
    const char *header = getenv("HTTP_AUTHORIZATION");

    if (header == NULL || *header == '\0')
        return 0;

    /* Aceptar tanto "Bearer <token>" como "<token>". */
    const char *token = header;

    if (strncmp(header, "Bearer ", 7) == 0)
        token = header + 7;

    if (*token == '\0')
        return 0;

    return session_validate(token);
}

int main(void)
{
    const char *method = getenv("REQUEST_METHOD");

    if (method == NULL || strcmp(method, "POST") != 0) {
        respond(405, "Method Not Allowed",
                "{\"status\":\"error\",\"message\":\"Use POST\"}");
        return 0;
    }

    if (!authorized()) {
        respond(401, "Unauthorized",
                "{\"status\":\"error\",\"message\":\"Sesion invalida o expirada\"}");
        return 0;
    }

    const char *length_str = getenv("CONTENT_LENGTH");

    if (length_str == NULL || *length_str == '\0') {
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"Falta el cuerpo JSON\"}");
        return 0;
    }

    char *end = NULL;
    long length = strtol(length_str, &end, 10);

    if (end == length_str || *end != '\0' || length <= 0 || length >= MAX_BODY_SIZE) {
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"Tamano de cuerpo invalido\"}");
        return 0;
    }

    char body[MAX_BODY_SIZE];
    size_t received = fread(body, 1, (size_t)length, stdin);
    body[received] = '\0';

    if (received != (size_t)length) {
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"Cuerpo incompleto\"}");
        return 0;
    }

    cJSON *json = cJSON_Parse(body);

    if (json == NULL) {
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"JSON invalido\"}");
        return 0;
    }

    cJSON *state = cJSON_GetObjectItemCaseSensitive(json, "state");

    if (!cJSON_IsString(state) || state->valuestring == NULL) {
        cJSON_Delete(json);
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"Se requiere state: on u off\"}");
        return 0;
    }

    int result;

    if (strcmp(state->valuestring, "on") == 0) {
        result = robot_vacuum_on();
    } else if (strcmp(state->valuestring, "off") == 0) {
        result = robot_vacuum_off();
    } else {
        cJSON_Delete(json);
        respond(400, "Bad Request",
                "{\"status\":\"error\",\"message\":\"state debe ser on u off\"}");
        return 0;
    }

    cJSON_Delete(json);

    if (result != 0) {
        respond(500, "Internal Server Error",
                "{\"status\":\"error\",\"message\":\"No se pudo controlar la aspiracion\"}");
        return 0;
    }

    respond(200, "OK",
            "{\"status\":\"ok\",\"message\":\"Estado de aspiracion actualizado\"}");
    return 0;
}