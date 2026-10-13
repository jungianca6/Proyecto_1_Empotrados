#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "session.h"

static void send_error(int http_code, const char *msg)
{
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n");
    printf("\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

static int get_token(char *token, size_t token_size)
{
    char *auth = getenv("HTTP_AUTHORIZATION");

    if (!auth || auth[0] == '\0')
        return -1;

    if (strncmp(auth, "Bearer ", 7) == 0)
        auth += 7;

    if (strlen(auth) >= token_size)
        return -1;

    strcpy(token, auth);
    return 0;
}

int main(void)
{
    char token[128];

    if (get_token(token, sizeof(token)) != 0 ||
        !session_validate(token)) {

        send_error(401, "Token invalido o expirado");
        return 0;
    }

    int result = system(
        "echo '=== ANTES ===' > /tmp/audio-debug.log; "
        "cat /proc/asound/cards >> /tmp/audio-debug.log 2>&1; "

        "echo '\n=== MODPROBE ===' >> /tmp/audio-debug.log; "
        "/sbin/modprobe snd_bcm2835 enable_headphones=1 enable_hdmi=0 "
        ">> /tmp/audio-debug.log 2>&1; "

        "echo '\n=== DESPUES ===' >> /tmp/audio-debug.log; "
        "cat /proc/asound/cards >> /tmp/audio-debug.log 2>&1; "

        "echo '\n=== MPG123 HEADPHONES ===' >> /tmp/audio-debug.log; "
        "/usr/bin/mpg123 -o alsa -a default:Headphones -v "
        "/usr/share/robot-audio/system-start.mp3 "
        ">> /tmp/audio-debug.log 2>&1"
    );

    if (result != 0) {

        FILE *f = fopen("/tmp/audio-debug.log", "r");
        char debug[4096] = {0};

        if (f) {
            size_t bytes_read = fread(debug, 1, sizeof(debug) - 1, f);
            debug[bytes_read] = '\0';
            fclose(f);
        }

        printf("Content-Type: text/plain\r\n");
        printf("\r\n");
        printf("Codigo: %d\n\n", result);
        printf("%s\n", debug);

        return 0;
    }

    printf("Content-Type: application/json\r\n");
    printf("\r\n");

    printf(
        "{"
        "\"status\":\"ok\","
        "\"message\":\"Audio reproducido\""
        "}\n"
    );

    return 0;
}