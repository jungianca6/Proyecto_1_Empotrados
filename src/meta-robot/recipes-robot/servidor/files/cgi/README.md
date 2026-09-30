# CGI del servidor — Estado de implementación

Este directorio contiene los binarios CGI que uhttpd ejecuta para atender
las peticiones del panel de control web. Cada uno sigue la arquitectura
definida en `docs/arquitectura/servidor_web.md`.

## Estado actual

| Archivo | Estado | Descripción |
|---|---|---|
| `login.c` | Implementado | Autenticación y generación de token de sesión |
| `sensors.c` | Implementado | Lectura de sensores de proximidad (front/left/right) |
| `leds.c` | Implementado (parcial, ver pendientes) | Consultar/cambiar estado de LEDs |
| `mode.c` | Implementado | Consultar/cambiar modo autónomo/manual |
| `motors.c` | Implementado | Control manual de motores |
| `status.c` | Implementado (parcial, ver pendientes) | Estado consolidado (modo + sensores + LEDs + audio) |
| `map.c` | Implementado (stub, ver pendientes) | Snapshot del mapa de recorrido |
| `audioplay.c`, `audiopause.c`, `audiostop.c`, `audiovolume.c`, `audiolist.c` | En progreso | A cargo de otro integrante, en rama aparte |

## Dependencia: `librobot_stub.c`

Todos los CGI ya implementados **enlazan contra el stub** en
`src/lib/stub/librobot_stub.c`, no contra la biblioteca real (`librobot.so`),
porque los módulos de motores/sensores/LEDs/audio de `librobot` todavía están
en desarrollo. El stub solo imprime la llamada
recibida por consola y devuelve valores fijos de éxito — sirve para validar
la lógica del CGI (parseo de JSON, autenticación, formato de respuesta) sin
depender de hardware real ni de que la biblioteca esté terminada.

**Antes de la entrega final es obligatorio reemplazar el enlace del stub por
la biblioteca real** en el `CMakeLists.txt` correspondiente. Mientras tanto,
si alguien recompila estos CGI y usa el stub sin darse cuenta en la imagen
Yocto, el robot no se moverá aunque el servidor responda "ok".

## Módulo de sesión (`src/lib/session/`)

`login.c`, `sensors.c`, `leds.c`, `mode.c`, `motors.c`, `status.c` y `map.c`
dependen de `session.h`/`session.c` para validar el token de autenticación
recibido en el header `Authorization`. El mecanismo:

1. `login.c` valida usuario/contraseña (actualmente fijos en el código:
   `admin` / `robot123` — **pendiente moverlo a config + hash antes de
   entregar**) y genera un token aleatorio guardado en
   `/tmp/sessions/<token>` con expiración de 1 hora.
2. Cada CGI protegido llama a `session_validate(token)` para verificar que
   el token existe y no ha expirado.

## Contrato de archivos compartidos en `/tmp`

Varios CGI se comunican con otros módulos (navegación, futuros procesos)
a través de archivos simples en `/tmp`, en vez de mantener estado en
memoria (recordar que cada CGI es un proceso efímero):

| Archivo | Escrito por | Leído por | Formato |
|---|---|---|---|
| `/tmp/sessions/<token>` | `login.c` | todos los CGI protegidos | `usuario\nexpiración_unix` |
| `/tmp/robot_mode` | `mode.c` | `mode.c`, `status.c` | texto plano: `autonomous` o `manual` |
| `/tmp/robot_map.json` | *(pendiente: módulo de navegación)* | `map.c` | JSON: `{"width":N,"height":N,"grid":[[...]]}` con `0`=desconocida, `1`=visitada, `2`=obstáculo |

## Cómo compilar y probar cada CGI manualmente (fuera de Yocto)

Requiere `libcjson-dev` instalado (`sudo apt install libcjson-dev`).

```bash
# login
gcc -o login_test cgi/login.c ../../../../../lib/session/session.c \
    -I../../../../../lib/session -lcjson

echo '{"username":"admin","password":"robot123"}' | CONTENT_LENGTH=44 ./login_test
```

```bash
# sensors / leds / motors / status (requieren token válido y el stub de librobot)
gcc -o sensors_test cgi/sensors.c ../../../../../lib/stub/librobot_stub.c \
    ../../../../../lib/session/session.c \
    -I../../../../../lib/include -I../../../../../lib/session -lcjson

TOKEN="<token obtenido de login_test>"
HTTP_AUTHORIZATION="$TOKEN" ./sensors_test
```

```bash
# mode / map (no dependen del stub de librobot, solo de session)
gcc -o map_test cgi/map.c ../../../../../lib/session/session.c \
    -I../../../../../lib/session -lcjson

HTTP_AUTHORIZATION="$TOKEN" ./map_test
```

Ajusta las rutas relativas (`-I../../../../../lib/...`) según desde dónde
ejecutes el comando.

## Pendientes conocidos

- [ ] **`leds.c` y `status.c` reportan LEDs con estado fijo/falso.** El
      header `librobot.h` actual no tiene una función para *leer* el estado
      de un LED (solo `robot_led_set()`). Hay que decidir si se agrega
      `robot_led_get()` a la API de la biblioteca, o si el estado se guarda
      del lado del servidor en un archivo similar a `mode.c`.
- [ ] **`status.c` reporta audio con estado fijo/falso** por el mismo
      motivo — falta una forma de consultar el estado real de reproducción.
- [ ] **`map.c` devuelve una grilla vacía de ejemplo** hasta que el módulo
      de navegación escriba `/tmp/robot_map.json` con el formato acordado
      en la tabla de "Contrato de archivos compartidos" arriba.
- [ ] **Credenciales de login hardcodeadas en texto plano.** Debe moverse a
      un archivo de configuración con la contraseña hasheada antes de la
      entrega (requerimiento de seguridad de la especificación).
- [ ] **Reemplazar el stub por `librobot.so` real** en el sistema de build
      una vez que los módulos del milestone 3 estén terminados.
- [ ] Los 5 CGI de audio están en desarrollo en otra rama por otro
      integrante del equipo.