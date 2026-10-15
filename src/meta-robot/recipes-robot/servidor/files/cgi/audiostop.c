#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{\"status\":\"ok\",\"message\":\"Detencion solicitada\"}\n"
    );

    return 0;
}