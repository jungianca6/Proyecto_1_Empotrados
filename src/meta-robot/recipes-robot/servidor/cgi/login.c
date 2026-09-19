#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf("{\"status\":\"ok\",\"message\":\"Login endpoint operativo\","
           "\"token\":\"test-token\"}\n");

    return 0;
}