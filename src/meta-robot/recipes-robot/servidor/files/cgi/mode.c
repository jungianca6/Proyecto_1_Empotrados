#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *method = getenv("REQUEST_METHOD");

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    if (method != NULL && strcmp(method, "POST") == 0) {
        printf(
            "{"
            "\"status\":\"ok\","
            "\"message\":\"Cambio de modo recibido\""
            "}\n"
        );
    } else {
        printf(
            "{"
            "\"status\":\"ok\","
            "\"data\":{"
                "\"mode\":\"manual\""
            "}"
            "}\n"
        );
    }

    return 0;
}