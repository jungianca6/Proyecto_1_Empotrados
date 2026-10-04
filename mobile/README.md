# Aplicación móvil Rover

Cliente Android/iOS desarrollado con React Native y Expo para controlar y monitorear el robot aspiradora.

## Requisitos

- Node.js LTS y npm.
- Teléfono con Expo Go o emulador Android/iOS.
- Servidor de la Raspberry Pi accesible desde la misma red Wi-Fi.

## Desarrollo

```bash
cd mobile
npm install
npm start
```

Escanea el QR desde Expo Go o ejecuta `npm run android` / `npm run ios` con un emulador configurado. La primera pantalla solicita la URL base del servidor, usuario y contraseña. Usa la dirección de la Raspberry Pi en la red local, por ejemplo `http://192.168.1.100`.

Para probar solo la interfaz, pulsa **Probar en modo demo** en la pantalla inicial. No requiere URL, credenciales, Raspberry Pi ni conexión de red; carga sensores, LEDs, mapa y canciones de muestra. El estado `DEMO LOCAL` confirma que los botones no envían comandos al robot.

## Inicio de sesión

Para conectarte al servidor del proyecto, usa la dirección IP de la Raspberry Pi y, mientras el CGI de login siga siendo el stub actual, puedes escribir cualquier usuario y contraseña; por ejemplo:

Usuario: demo
Contraseña: demo

El handler actual no valida esos datos: responde siempre con el token fijo `test-token`. `demo` / `demo` son solo valores de prueba, no una cuenta registrada ni credenciales seguras. La cuenta de Expo Go/Expo CLI es independiente y no sirve para iniciar sesión en Rover. Para probar sin servidor, usa **Probar en modo demo** y no ingreses credenciales.

## Funciones

- Inicio de sesión y URL configurable del servidor.
- Conmutación entre control manual y navegación autónoma.
- Comandos direccionales, lecturas de sensores y estado de los cuatro LEDs.
- Mapa 2D actualizado mediante consulta periódica.
- Lista de canciones, reproducción, pausa, detención y volumen.
- Actualización de telemetría cada tres segundos y estado de conexión visible.

## API esperada

La app consulta `/cgi-bin/status`, `sensors`, `leds`, `map` y `audiolist`. Envía solicitudes `POST` con campos `application/x-www-form-urlencoded` a `login`, `mode`, `motors`, `audioplay`, `audiopause`, `audiostop` y `audiovolume`. Las respuestas de lectura siguen la estructura `{ "status": "ok", "data": ... }`; el inicio de sesión debe incluir `token`.

La app está conectada a los endpoints actuales, pero los handlers del servidor en el repositorio aún devuelven respuestas de prueba: el login no valida credenciales y los comandos no accionan el hardware. Se requiere completar esos handlers y su integración con `librobot` antes de usarla para controlar el robot real. El mapa espera celdas `0` desconocida, `1` visitada, `2` obstáculo y `3` posición del robot.