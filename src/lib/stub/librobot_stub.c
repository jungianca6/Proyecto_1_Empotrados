#include <stdio.h>
#include "../include/librobot.h"

// --- Motores ---
int robot_motor_init(void) {
    printf("[STUB] robot_motor_init()\n");
    return 0;
}

int robot_motor_deinit(void) {
    printf("[STUB] robot_motor_deinit()\n");
    return 0;
}

int robot_motor_set(motor_id_t motor, int speed) {
    printf("[STUB] robot_motor_set(motor=%d, speed=%d)\n", motor, speed);
    return 0;
}

int robot_motor_stop_all(void) {
    printf("[STUB] robot_motor_stop_all()\n");
    return 0;
}

// --- Sensores ---
int robot_sensor_init(void) {
    printf("[STUB] robot_sensor_init()\n");
    return 0;
}

int robot_sensor_deinit(void) {
    printf("[STUB] robot_sensor_deinit()\n");
    return 0;
}

float robot_sensor_read_distance(sensor_id_t sensor) {
    printf("[STUB] robot_sensor_read_distance(sensor=%d)\n", sensor);
    return 42.0f; // valor falso fijo para pruebas
}

// --- LEDs ---
int robot_led_init(void) {
    printf("[STUB] robot_led_init()\n");
    return 0;
}

int robot_led_deinit(void) {
    printf("[STUB] robot_led_deinit()\n");
    return 0;
}

int robot_led_set(led_id_t led, int state) {
    printf("[STUB] robot_led_set(led=%d, state=%d)\n", led, state);
    return 0;
}

// --- Audio ---
int robot_audio_init(void) {
    printf("[STUB] robot_audio_init()\n");
    return 0;
}

int robot_audio_deinit(void) {
    printf("[STUB] robot_audio_deinit()\n");
    return 0;
}

int robot_audio_play(const char *filepath) {
    printf("[STUB] robot_audio_play(%s)\n", filepath);
    return 0;
}

int robot_audio_pause(void) {
    printf("[STUB] robot_audio_pause()\n");
    return 0;
}

int robot_audio_stop(void) {
    printf("[STUB] robot_audio_stop()\n");
    return 0;
}

int robot_audio_set_volume(int volume) {
    printf("[STUB] robot_audio_set_volume(%d)\n", volume);
    return 0;
}