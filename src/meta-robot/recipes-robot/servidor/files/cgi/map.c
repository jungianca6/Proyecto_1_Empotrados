#include <stdio.h>
#include <stdlib.h>
#include "session.h"

#define MAP_FILE "/tmp/robot_map.json"

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

    FILE *f = fopen(MAP_FILE, "r");
    if (!f) {
        // Stub: no existe el archivo todavía, devolvemos una grilla vacía de ejemplo.
        printf("Content-Type: application/json\r\n\r\n");
        printf(
            "{\"status\":\"ok\",\"data\":{"
            "\"width\":5,\"height\":5,"
            "\"grid\":["
            "[0,0,0,0,0],"
            "[0,0,0,0,0],"
            "[0,0,0,0,0],"
            "[0,0,0,0,0],"
            "[0,0,0,0,0]"
            "]}}"
        );
        return 0;
    }

    // Si el archivo existe, se asume que ya contiene JSON válido escrito
    // por el módulo de navegación, y se reenvía tal cual.
    printf("Content-Type: application/json\r\n\r\n");
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        fwrite(buffer, 1, n, stdout);
    }
    fclose(f);
    return 0;
}