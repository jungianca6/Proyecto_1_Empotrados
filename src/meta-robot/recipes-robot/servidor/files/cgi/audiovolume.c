#include <stdio.h>
#include "audio_cgi.h"

int main(void)
{
    const char *volume = audio_cgi_query_value("value");
    int result = volume == NULL ? -1 : audio_cgi_call("volume", volume);

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(result == 0
        ? "{\"status\":\"ok\",\"message\":\"Volumen actualizado\"}\n"
        : "{\"status\":\"error\",\"message\":\"Volumen invalido\"}\n");

    return result == 0 ? 0 : 1;
}