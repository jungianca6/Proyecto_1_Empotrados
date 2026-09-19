#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"autonomous\":false,"
            "\"manual\":true,"
            "\"obstacle\":false,"
            "\"system\":true"
        "}"
        "}\n"
    );

    return 0;
}