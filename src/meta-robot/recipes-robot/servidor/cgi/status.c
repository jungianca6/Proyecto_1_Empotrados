#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"mode\":\"manual\","
            "\"motor\":\"stopped\","
            "\"server\":\"running\""
        "}"
        "}\n"
    );

    return 0;
}