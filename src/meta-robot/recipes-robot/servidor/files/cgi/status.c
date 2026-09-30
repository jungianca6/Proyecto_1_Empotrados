#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "librobot.h"
#include "session.h"

#define MODE_FILE "/tmp/robot_mode"

static void send_error(int http_code, const char *msg) {
    printf("Status: %d\r\n", http_code);
    printf("Content-Type: application/json\r\n\r\n");
    printf("{\"status\":\"error\",\"message\":\"%s\"}", msg);
}

static void read_mode(char *out, int out_len) {
    FILE *f = fopen(MODE_FILE, "r");
    if (!f) {
        snprintf(out, out_len, "autonomous");
        return;
    }
    if (fscanf(f, "%63s", out) != 1) {
        snprintf(out, out_len, "autonomous");
    }
    fclose(f);
}

int main(void) {
    char *auth = getenv("HTTP_AUTHORIZATION");
    if (!auth || !session_validate(auth)) {
        send_error(401, "Token inválido o expirado");
        return 0;
    }

    char mode[64];
    read_mode(mode, sizeof(mode));

    float front = robot_sensor_read_distance(SENSOR_FRONT);
    float left  = robot_sensor_read_distance(SENSOR_LEFT);
    float right = robot_sensor_read_distance(SENSOR_RIGHT);

    // TODO: estado real de LEDs y audio cuando existan las funciones
    // robot_led_get() / robot_audio_get_status() en librobot.h.
    printf("Content-Type: application/json\r\n\r\n");
    printf(
        "{\"status\":\"ok\",\"data\":{"
        "\"mode\":\"%s\","
        "\"sensors\":{\"front\":%.2f,\"left\":%.2f,\"right\":%.2f},"
        "\"leds\":{\"auto\":0,\"manual\":0,\"obstacle\":0,\"power\":1},"
        "\"audio\":{\"playing\":false,\"volume\":0}"
        "}}",
        mode, front, left, right
    );
    return 0;
}