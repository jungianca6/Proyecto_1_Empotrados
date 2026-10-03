#ifndef LIBROBOT_H
#define LIBROBOT_H

// Convención de retorno: 0 = éxito, -1 = error, en todas las funciones int.

// --- Motores ---
typedef enum { MOTOR_LEFT, MOTOR_RIGHT } motor_id_t;

int robot_motor_init(void);
int robot_motor_deinit(void);
int robot_motor_set(motor_id_t motor, int speed); // speed: -100 a 100 (signo = dirección)
int robot_motor_stop_all(void);

// --- Sensores de proximidad ---
typedef enum { SENSOR_FRONT, SENSOR_LEFT, SENSOR_RIGHT } sensor_id_t;

int robot_sensor_init(void);
int robot_sensor_deinit(void);
float robot_sensor_read_distance(sensor_id_t sensor); // cm, o -1.0 si error

// --- LEDs ---
typedef enum { LED_AUTO, LED_MANUAL, LED_OBSTACLE, LED_POWER } led_id_t;

int robot_led_init(void);
int robot_led_deinit(void);
int robot_led_set(led_id_t led, int state); // 0 = apagado, 1 = encendido

// --- Audio ---
int robot_audio_init(void);
int robot_audio_deinit(void);
int robot_audio_play(const char *filepath);
int robot_audio_pause(void);
int robot_audio_stop(void);
int robot_audio_set_volume(int volume); // 0-100

#endif 
