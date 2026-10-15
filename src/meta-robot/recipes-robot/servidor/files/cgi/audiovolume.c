#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{\"status\":\"ok\",\"message\":\"Cambio de volumen recibido\"}\n"
    );

    return 0;
}