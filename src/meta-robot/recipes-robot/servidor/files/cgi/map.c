#include <stdio.h>
#include <stdlib.h>
#include "session.h"

/*
 * GET /cgi-bin/map
 *
 * El proceso de navegacion (app_robot) escribe /tmp/robot_map.json de forma
 * atomica; aqui solo se reenvia envuelto en el formato estandar de la API:
 *
 *   {"status":"ok","data": <contenido de robot_map.json>}
 *
 * Formato de "data" (ver docs/arquitectura/mapeo.md):
 *   seq, cell_cm, width, height, origin{col,row},
 *   robot{x,y,theta_deg,col,row}, stats{...},
 *   grid[height][width] con 0 = desconocida, 1 = visitada, 2 = obstaculo
 */

#define MAP_FILE "/tmp/robot_map.json"

static void send_error(int http_code, const char *msg)
{
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

int main(void)
{
    char *auth = getenv("HTTP_AUTHORIZATION");

    if (!auth || !session_validate(auth)) {
        send_error(401, "Token invalido o expirado");
        return 0;
    }

    FILE *f = fopen(MAP_FILE, "r");

    if (!f) {
        /* La navegacion todavia no ha generado el mapa. */
        send_error(503, "Mapa no disponible: la navegacion no esta en ejecucion");
        return 0;
    }

    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"ok\",\"data\":");

    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0)
        fwrite(buffer, 1, n, stdout);

    fclose(f);
    printf("}");
    return 0;
}
