# API de `librobot`

Biblioteca dinámica que encapsula el acceso al hardware del robot: motores,
sensores de proximidad, LEDs y audio. Consumida por los binarios CGI del
servidor web (`src/meta-robot/recipes-robot/servidor/files/cgi/`).

## Convenciones generales
- Todas las funciones de tipo `int` retornan `0` en éxito y `-1` en error.
- Cada módulo se inicializa y libera por separado: `robot_<modulo>_init()` /
  `robot_<modulo>_deinit()`. Debe llamarse `init()` antes de usar cualquier
  otra función del módulo, y `deinit()` antes de terminar el programa para
  liberar los GPIO correctamente.
- Header público: `include/librobot.h`.

## Módulo: Motores

```c
int robot_motor_init(void);
int robot_motor_deinit(void);
int robot_motor_set(motor_id_t motor, int speed); // speed: -100 a 100
int robot_motor_stop_all(void);
```

- `motor`: `MOTOR_LEFT` o `MOTOR_RIGHT`.
- `speed`: rango -100 a 100. El signo define la dirección (negativo = reversa),
  la magnitud define el duty cycle del PWM.

Ejemplo:
```c
robot_motor_init();
robot_motor_set(MOTOR_LEFT, 80);
robot_motor_set(MOTOR_RIGHT, 80);
// ...
robot_motor_stop_all();
robot_motor_deinit();
```

## Módulo: Sensores de proximidad

```c
int robot_sensor_init(void);
int robot_sensor_deinit(void);
float robot_sensor_read_distance(sensor_id_t sensor); // cm, -1.0 si error
```

- `sensor`: `SENSOR_FRONT`, `SENSOR_LEFT` o `SENSOR_RIGHT`.
- Retorna la distancia en centímetros, o `-1.0` si la lectura falló.

## Módulo: LEDs

```c
int robot_led_init(void);
int robot_led_deinit(void);
int robot_led_set(led_id_t led, int state); // 0 = apagado, 1 = encendido
```

- `led`: `LED_AUTO`, `LED_MANUAL`, `LED_OBSTACLE` o `LED_POWER`.

## Módulo: Audio

```c
int robot_audio_init(void);
int robot_audio_deinit(void);
int robot_audio_play(const char *filepath);
int robot_audio_pause(void);
int robot_audio_stop(void);
int robot_audio_set_volume(int volume); // 0-100
```

- `filepath`: ruta absoluta al archivo `.mp3` a reproducir.

## Stub para desarrollo (`stub/librobot_stub.c`)

Mientras los módulos reales de motores/sensores/LEDs/audio no estén listos, existe una implementación de prueba en
`stub/librobot_stub.c` que imprime cada llamada por consola y devuelve
valores fijos de éxito, para poder compilar y probar los CGI de forma
aislada. **No debe usarse en la imagen final** — el CMake de producción debe
enlazar contra la biblioteca real, no contra el stub.