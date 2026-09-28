#include <stdio.h>
#include "audio_cgi.h"

int main(void)
{
    const char *path = audio_cgi_query_value("path");
    int result = path == NULL ? -1 : audio_cgi_call("play", path);

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(result == 0
        ? "{\"status\":\"ok\",\"message\":\"Reproduccion solicitada\"}\n"
        : "{\"status\":\"error\",\"message\":\"No se pudo reproducir\"}\n");

    return result == 0 ? 0 : 1;
}