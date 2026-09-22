#include <stdio.h>

int main(void)
{
    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"data\":{"
            "\"width\":5,"
            "\"height\":5,"
            "\"cells\":["
                "[0,0,0,0,0],"
                "[0,0,1,0,0],"
                "[0,0,1,0,0],"
                "[0,0,0,0,0],"
                "[0,0,0,0,0]"
            "]"
        "}"
        "}\n"
    );

    return 0;
}