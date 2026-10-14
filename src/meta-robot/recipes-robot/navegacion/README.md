# Navegación autónoma, log de eventos y mapa

Este directorio contiene `app_robot`, el proceso que ejecuta la navegación
autónoma (la FSM de `librobot`). Además de mover el robot, ahora:

1. escribe un **log de eventos** de la navegación (`/tmp/robot_events.log`);
2. construye un **mapa** del recorrido por odometría estimada y lo publica en
   `/tmp/robot_map.json`, que el servidor entrega en `GET /cgi-bin/map`.

El diseño completo (modelo de movimiento, contrato del JSON, calibración y
limitaciones) está en [`docs/arquitectura/mapeo.md`](../../../../docs/arquitectura/mapeo.md).
Este README explica cómo trabajar con el código.

## Estado

| Parte | Estado |
|---|---|
| Log de eventos y mapa | Probado solo en el simulador del host |
| Compilación con `bitbake navegacion` | Sin probar |
| Ejecución en QEMU / Raspberry Pi | Sin probar |
| Calibración de parámetros | Pendiente (valores provisionales) |

Sin calibrar, el mapa no es confiable. Ver "Calibración" más abajo.

## Qué cambió respecto a antes

Archivos nuevos en `files/` y `files/src/`:

| Archivo | Qué hace |
|---|---|
| `src/mapper.c`, `src/mapper.h` | Pose estimada y grilla 2D. No toca hardware: recibe eventos con marca de tiempo. |
| `src/nav_events.c`, `src/nav_events.h` | Se engancha a la FSM (sin modificarla): detecta cambios de estado, escribe el log y alimenta al mapper. |
| `robot_map.conf` | Parámetros de calibración. Se instala en `/etc/robot_map.conf`. |

Archivos existentes modificados:

| Archivo | Cambio |
|---|---|
| `src/main.c` | 4 cambios pequeños: `#include "nav_events.h"`, `nav_events_init()` tras `fsm_init()`, `nav_events_tick()` en el lazo principal, y `nav_events_tick()` más `nav_events_close()` al salir. |
| `CMakeLists.txt` | Agrega `mapper.c`, `nav_events.c`, el include de `src` y `-lm`. |
| `navegacion_1.0.bb` | Agrega las fuentes a `SRC_URI` e instala `robot_map.conf`. |

Fuera de este directorio:

| Archivo | Cambio |
|---|---|
| `servidor/files/cgi/map.c` | Devuelve `{"status":"ok","data":<mapa>}`; responde 503 si no hay mapa. |
| `servidor/files/www/js/map_view.js` | `drawMap(canvas, data)` para la página web (aún sin conectar a `index.html`). |
| `mobile/src/api.ts`, `mobile/App.tsx` | La app convierte el mapa al formato que ya dibujaba y calcula el tamaño de celda. |
| `tools/map_sim/` | Simulador y herramienta de replay (ver abajo). |

## Cómo funciona

```
fsm_update() ──> nav_events_tick() ──> mapper ──> /tmp/robot_map.json  ──> cgi-bin/map ──> web / app
                      │
                      └──> /tmp/robot_events.log
```

`nav_events_tick()` se llama en cada vuelta del lazo, después de `fsm_update()`.
Compara `fsm_get_state()` con el estado anterior. Si cambió, escribe una línea
de log y le avisa al mapper el nuevo movimiento. Cuando el estado pasa de
`MOVING_FORWARD` a `AVOID_STOP` lee los dos sensores y registra el obstáculo.
El mapa se reescribe cada 500 ms, solo si cambió, de forma atómica (archivo
temporal más `rename`), así el CGI nunca lee un JSON a medias.

Como los motores no usan PWM, el movimiento se deduce solo del estado de la FSM:

| Estado | Movimiento |
|---|---|
| `MOVING_FORWARD` | avance a `speed_cm_s` |
| `BACKING_UP` | retroceso a `speed_cm_s` |
| `TURNING_LEFT` / `TURNING_RIGHT` | giro sobre una rueda (arco alrededor de la rueda quieta) |
| cualquier otro | detenido |

## Formato del log

Una línea JSON por evento. `t` son milisegundos desde que arrancó `app_robot`.
El log se reinicia en cada arranque y rota a `.1` al pasar 1 MB.

```json
{"t":0,"ev":"start","params":{"cell_cm":5.0,"speed_cm_s":20.0,"turn_rate_deg_s":80.0,...}}
{"t":10,"ev":"state","from":"IDLE","to":"MOVING_FORWARD"}
{"t":6000,"ev":"state","from":"MOVING_FORWARD","to":"AVOID_STOP"}
{"t":6000,"ev":"obstacle","front":1,"left":0,"x":0.0,"y":119.8,"th":90.0}
{"t":120000,"ev":"end"}
```

El evento `start` guarda los parámetros con que se calculó el mapa en vivo.
`x`, `y` (cm) y `th` (grados) son la pose estimada, no la real.

## Probar sin robot

Solo necesita `gcc` y `make` en el host (Ubuntu). No requiere Yocto ni QEMU.

```bash
cd tools/map_sim
make                 # compila map_sim y map_replay
make run             # 120 s virtuales, dibuja el mapa en texto
make run-drift       # el robot "real" difiere del modelo: muestra la deriva
./map_sim --secs 300 --seed 4 --out mapa.json --log eventos.log --ascii
```

Opciones de `map_sim`: `--secs N`, `--seed N`, `--mismatch`, `--out archivo`,
`--log archivo`, `--ascii`.

El simulador ejecuta el `fsm.c` real, sin modificar, y los mismos `mapper.c` y
`nav_events.c` del robot. Solo sustituye el hardware: un robot virtual en una
sala con dos cajas, sensores digitales simulados y un reloj virtual. Con
parámetros iguales al modelo debería dar, con la semilla 1:

```
celdas visitadas en el mapa:      1300 (99.5 % coinciden con lo realmente recorrido)
celdas de obstaculo en el mapa:   19 (100.0 % cerca de una pared o caja real)
error de pose final:              0.2 cm, 0.0 grados
```

Si estos números cambian después de tocar el código, algo se rompió (o cambió
a propósito: revisen qué).

### Reconstruir un mapa desde un log

Sirve para ajustar la calibración sin volver a correr el robot:

```bash
tools/map_sim/map_replay /tmp/robot_events.log -c mi_robot_map.conf -o mapa.json --ascii
```

`-c` es opcional (usa los valores por defecto). `mi_robot_map.conf` son líneas
`clave=valor`, las mismas que `robot_map.conf`.

## Compilar con Yocto

```bash
bitbake navegacion
```

La receta ya declara las fuentes nuevas. Si falla por una cabecera o por `-lm`,
revisen primero `SRC_URI` y el `target_link_libraries` del `CMakeLists.txt`.

## Calibración

Los valores de `robot_map.conf` son provisionales. Hay que medirlos con el
robot real (el procedimiento paso a paso está en `docs/arquitectura/mapeo.md`):

| Parámetro | Qué medir |
|---|---|
| `speed_cm_s` | distancia recorrida en 3 s de avance, dividida entre 3 |
| `turn_rate_deg_s` | ángulo girado en 3 s sobre una rueda, dividido entre 3 |
| `pivot_offset_cm` | mitad de la separación entre ruedas |
| `robot_radius_cm` | radio del robot |
| `front_obstacle_cm`, `side_obstacle_cm` | distancia del centro del robot al obstáculo cuando se enciende el LED del FC-51 |
| `turn_sign` | `-1` si `TURN_RIGHT` gira hacia la izquierda en el robot real |

Para probar un valor sin recompilar: editar `/etc/robot_map.conf` en el robot y
reiniciar `app_robot`. Para ajustar con datos reales: copiar
`/tmp/robot_events.log` al host y repetir con `map_replay`.

## Si modifican la navegación

El mapa depende de cómo se mueve el robot. Si cambian algo de esto, actualicen
también el mapa, o quedará desfasado sin avisar:

- **Nuevos estados o nombres en `fsm.h`:** actualizar `state_name()` en
  `nav_events.c` y `mapper_motion_from_name()` en `mapper.c`.
- **PWM o velocidades distintas por movimiento:** hoy se supone una sola
  velocidad de avance y una de giro; habría que pasar la velocidad al mapper.
- **Cambiar los FC-51 por sensores de distancia:** `mapper_obstacle()` marca una
  celda a distancia fija; con distancia real habría que usarla.
- **Nuevas funciones de hardware en `fsm.c`:** el simulador se enlaza con el
  `fsm.c` real, así que agreguen su versión simulada en `tools/map_sim/sim.c`
  (como `led_obstacle_set` y `robot_move`) o no compilará.
- **Nuevas cabeceras que `fsm.c` incluya:** agregar una copia vacía en
  `tools/map_sim/shim/` si no están en `librobot/files/include`.

## Limitaciones conocidas

- **Deriva.** Sin encoders ni IMU, la pose se estima solo con tiempo y
  velocidad. Cada contacto con una pared o deslizamiento de ruedas suma error
  que nada corrige.
- **Sensores digitales.** Cada detección marca una sola celda a distancia fija.
  Las paredes salen punteadas.
- **Lectura doble de sensores.** `fsm.c` decide el obstáculo internamente y no
  expone qué leyó, así que `nav_events.c` vuelve a leer los sensores justo
  después de la transición. Si el obstáculo desaparece en esa fracción de
  segundo, el evento sale con `front:0,left:0` y no se marca nada. Se resuelve
  si `fsm.c` guarda y expone su última lectura.
- **Modo manual.** Los comandos manuales los ejecuta el CGI, no `app_robot`, así
  que no se registran. Al volver a autónomo la pose puede estar desfasada.
- **Reinicio.** Cada arranque de `app_robot` reinicia el mapa y la pose.
- **Solo sensor lateral izquierdo.** No se mapean obstáculos a la derecha.
- **`/tmp`.** El log y el mapa están en RAM y se pierden al reiniciar.

## Pendientes

- [ ] Compilar con `bitbake` y ejecutar `app_robot` en QEMU o en la Raspberry Pi.
- [ ] Calibrar `robot_map.conf` con el robot real.
- [ ] Conectar `map_view.js` a `index.html` y `app.js` (mapa en la página web).
- [ ] Probar el mapa de la app móvil en un teléfono y ajustar el ancho de celda.
- [ ] Exponer la última lectura de sensores desde `fsm.c` (ver limitaciones).
