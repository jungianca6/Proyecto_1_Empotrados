#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"front\":false,"
            "\"left\":false,"
            "\"right\":false"
        "}"
        "}\n"
    );

    return 0;
}