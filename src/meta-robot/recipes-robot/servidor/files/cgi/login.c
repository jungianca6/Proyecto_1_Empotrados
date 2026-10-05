#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "database.h"
#include "session.h"

static void send_error(
    int http_code,
    const char *message)
{
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");

    printf(
        "{"
        "\"status\":\"error\","
        "\"message\":\"%s\""
        "}\n",
        message
    );
}

int main(void)
{
    char *len_str = getenv("CONTENT_LENGTH");

    if (!len_str) {
        send_error(
            400,
            "Body ausente"
        );
        return 0;
    }

    int len = atoi(len_str);

    if (len <= 0 || len >= 1024) {
        send_error(
            400,
            "Body inválido"
        );
        return 0;
    }

    char body[1024] = {0};

    size_t read_bytes = fread(
        body,
        1,
        (size_t)len,
        stdin
    );

    if (read_bytes != (size_t)len) {
        send_error(
            400,
            "No se pudo leer el body"
        );
        return 0;
    }

    cJSON *json = cJSON_Parse(body);

    if (!json) {
        send_error(
            400,
            "JSON inválido"
        );
        return 0;
    }

    cJSON *user =
        cJSON_GetObjectItemCaseSensitive(
            json,
            "username"
        );

    cJSON *pass =
        cJSON_GetObjectItemCaseSensitive(
            json,
            "password"
        );

    if (!cJSON_IsString(user) ||
        !cJSON_IsString(pass)) {

        cJSON_Delete(json);

        send_error(
            400,
            "Faltan username o password"
        );

        return 0;
    }

    const char *username = user->valuestring;
    const char *password = pass->valuestring;

    /*
     * Inicializar la base de datos.
     */
    if (db_init() != 0) {
        cJSON_Delete(json);

        send_error(
            500,
            "No se pudo inicializar la base de datos"
        );

        return 0;
    }

    /*
     * Validar usuario y contraseña.
     *
     * db_validate_user() calcula MD5 de la contraseña
     * recibida y lo compara con el hash almacenado.
     */
    int valid =
        db_validate_user(
            username,
            password
        );

    if (valid < 0) {
        cJSON_Delete(json);

        send_error(
            500,
            "Error consultando la base de datos"
        );

        return 0;
    }

    if (valid == 0) {
        cJSON_Delete(json);

        send_error(
            401,
            "Usuario o contraseña incorrectos"
        );

        return 0;
    }

    /*
     * Credenciales correctas:
     * crear sesión temporal.
     */
    char token[65];

    if (session_create(
            username,
            token,
            sizeof(token)) != 0) {

        cJSON_Delete(json);

        send_error(
            500,
            "No se pudo crear la sesión"
        );

        return 0;
    }

    cJSON_Delete(json);

    printf(
        "Content-Type: application/json\r\n\r\n"
    );

    printf(
        "{"
        "\"status\":\"ok\","
        "\"message\":\"Login correcto\","
        "\"token\":\"%s\""
        "}\n",
        token
    );

    return 0;
}