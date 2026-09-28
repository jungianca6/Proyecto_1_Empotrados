#include <stdio.h>
#include "audio_cgi.h"

int main(void)
{
    int result = audio_cgi_call("pause", NULL);

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(result == 0
        ? "{\"status\":\"ok\",\"message\":\"Pausa solicitada\"}\n"
        : "{\"status\":\"error\",\"message\":\"No se pudo pausar\"}\n");

    return result == 0 ? 0 : 1;
}