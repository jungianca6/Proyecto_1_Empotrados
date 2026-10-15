#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"songs\":["
                "{\"id\":1,\"name\":\"test.mp3\"},"
                "{\"id\":2,\"name\":\"song2.mp3\"}"
            "]"
        "}"
        "}\n"
    );

    return 0;
}