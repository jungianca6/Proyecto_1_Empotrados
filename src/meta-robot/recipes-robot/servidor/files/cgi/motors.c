#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *method = getenv("REQUEST_METHOD");

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    if (method == NULL || method[0] == '\0') {
        printf(
            "{\"status\":\"error\",\"message\":\"Metodo HTTP no especificado\"}\n"
        );
        return 0;
    }

    printf(
        "{"
        "\"status\":\"ok\","
        "\"message\":\"Comando de motores recibido\","
        "\"method\":\"%s\""
        "}\n",
        method
    );

    return 0;
}