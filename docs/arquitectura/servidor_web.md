# Arquitectura del servidor web

## Stack tecnológico
- **Lenguaje:** C
- **Servidor HTTP:** uhttpd (servidor embebido de OpenWrt, ~22 KB, ideal para rootfs limitado)
- **Modelo de ejecución:** CGI — cada endpoint es un binario en `/www/cgi-bin/`
- **Formato de datos:** JSON (generado/parseado con cJSON)
- **Tiempo real (sensores/mapa):** Polling desde el cliente vía `fetch()` cada 300–500 ms

Justificación: uhttpd cumple con el presupuesto de rootfs (200 MB) por su tamaño mínimo,
y su modelo CGI permite que cada binario enlace directamente contra `librobot.so`
sin necesidad de bindings ni capas de traducción.

## Arquitectura de comunicación cliente-servidor
El cliente (navegador en PC o móvil) se conecta a la Raspberry Pi a través de la misma
red Wi-Fi local. Se sirve una única página web responsive (HTML/CSS/JS) desde uhttpd,
que funciona igual en ambas plataformas sin necesidad de una app nativa aparte.

## Endpoints (binarios CGI en `/www/cgi-bin/`)

| Endpoint | Método | Descripción |
|---|---|---|
| `/cgi-bin/login` | POST | Autenticación, genera token de sesión |
| `/cgi-bin/mode` | GET/POST | Consultar o cambiar modo (autónomo/manual) |
| `/cgi-bin/motors` | POST | Comando manual de movimiento + velocidad |
| `/cgi-bin/sensors` | GET | Última lectura de sensores (consumido por polling) |
| `/cgi-bin/leds` | GET | Estado actual de los 4 LEDs |
| `/cgi-bin/audio_list` | GET | Lista de canciones disponibles |
| `/cgi-bin/audio_play` | POST | Reproducir canción por ID |
| `/cgi-bin/audio_pause` / `/cgi-bin/audio_stop` | POST | Control de reproducción |
| `/cgi-bin/audio_volume` | POST | Ajustar volumen |
| `/cgi-bin/map` | GET | Snapshot del mapa (consumido por polling) |

## Autenticación
1. Usuario/contraseña validados en `/cgi-bin/login` (contraseña almacenada con hash, nunca en texto plano).
2. Al validar, se genera un token de sesión aleatorio y se guarda en un archivo bajo `/tmp/sessions/<token>`
   con su tiempo de expiración (ya que cada CGI es un proceso nuevo sin memoria persistente entre llamadas).
3. Cada CGI subsiguiente valida el token recibido (header o cookie) contra ese archivo antes de ejecutar la acción.
4. Tokens expirados o inválidos devuelven `401`.

## Interacción con `librobot.so`
Cada binario CGI se compila con CMake y se enlaza dinámicamente contra `librobot.so`.
Al recibir la petición, el CGI llama directamente a las funciones expuestas por la
biblioteca (ej. `robot_motor_set()`, `robot_sensor_read()`, `robot_audio_play()`),
sin capas intermedias de traducción.

## Formato de solicitudes, respuestas y errores
Todas las respuestas son JSON con la siguiente convención:

Éxito:
```json
{ "status": "ok", "data": { } }
```

Error:
```json
{ "status": "error", "message": "Token inválido o expirado" }
```

Códigos HTTP usados: `200` (ok), `400` (payload inválido), `401` (no autenticado),
`404` (ruta inexistente), `500` (error interno/hardware).

## Flujo general
```mermaid
flowchart TD
    A["Navegador PC/móvil"] -->|"HTTP (JSON) sobre Wi-Fi<br/>peticiones + polling periódico"| B["uhttpd"]
    B -->|"ejecuta binario CGI correspondiente"| C["Binario CGI en C"]
    C -->|"llamada directa a función de librobot.so"| D["librobot.so"]
    D -->|"acceso GPIO/PWM<br/>(ej. libgpiod)"| E["Hardware: motores, sensores, LEDs, audio"]
