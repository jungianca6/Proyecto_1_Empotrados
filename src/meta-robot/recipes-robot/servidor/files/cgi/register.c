#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "database.h"

static void send_json(
    int http_code,
    const char *status,
    const char *message)
{
    if (http_code != 200)
        printf("Status: %d\r\n", http_code);

    printf("Content-Type: application/json\r\n\r\n");

    printf(
        "{"
        "\"status\":\"%s\","
        "\"message\":\"%s\""
        "}\n",
        status,
        message
    );
}

int main(void)
{
    char *len_str = getenv("CONTENT_LENGTH");

    if (!len_str) {
        send_json(
            400,
            "error",
            "Body ausente"
        );
        return 0;
    }

    int len = atoi(len_str);

    if (len <= 0 || len >= 1024) {
        send_json(
            400,
            "error",
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
        send_json(
            400,
            "error",
            "No se pudo leer el body"
        );
        return 0;
    }

    cJSON *json = cJSON_Parse(body);

    if (!json) {
        send_json(
            400,
            "error",
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

        send_json(
            400,
            "error",
            "Faltan username o password"
        );

        return 0;
    }

    const char *username = user->valuestring;
    const char *password = pass->valuestring;

    /*
     * Validaciones sencillas para el proyecto.
     */

    size_t username_len = strlen(username);
    size_t password_len = strlen(password);

    if (username_len < 3 || username_len > 32) {
        cJSON_Delete(json);

        send_json(
            400,
            "error",
            "El usuario debe tener entre 3 y 32 caracteres"
        );

        return 0;
    }

    if (password_len < 4 || password_len > 128) {
        cJSON_Delete(json);

        send_json(
            400,
            "error",
            "La contraseña debe tener entre 4 y 128 caracteres"
        );

        return 0;
    }

    /*
     * Por simplicidad, no permitimos espacios en el nombre
     * de usuario.
     */
    for (size_t i = 0; i < username_len; i++) {
        if (username[i] == ' ' ||
            username[i] == '\t' ||
            username[i] == '\n' ||
            username[i] == '\r') {

            cJSON_Delete(json);

            send_json(
                400,
                "error",
                "El usuario no puede contener espacios"
            );

            return 0;
        }
    }

    if (db_init() != 0) {
        cJSON_Delete(json);

        send_json(
            500,
            "error",
            "No se pudo inicializar la base de datos"
        );

        return 0;
    }

    int result =
        db_create_user(
            username,
            password
        );

    cJSON_Delete(json);

    if (result == 1) {
        send_json(
            409,
            "error",
            "El usuario ya existe"
        );

        return 0;
    }

    if (result != 0) {
        send_json(
            500,
            "error",
            "No se pudo registrar el usuario"
        );

        return 0;
    }

    send_json(
        201,
        "ok",
        "Usuario registrado correctamente"
    );

    return 0;
}