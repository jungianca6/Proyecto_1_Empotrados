#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"message\":\"Solicitud de reproduccion recibida\""
        "}\n"
    );

    return 0;
}