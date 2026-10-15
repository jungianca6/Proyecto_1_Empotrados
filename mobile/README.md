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

Para conectarte al servidor del proyecto, usa la dirección IP de la Raspberry Pi y las credenciales de prueba configuradas en el CGI:

Usuario: admin
Contraseña: robot123

Estas credenciales están escritas como valores fijos en el handler de desarrollo; no deben usarse en un despliegue público. La cuenta de Expo Go/Expo CLI es independiente y no sirve para iniciar sesión en Rover. Para probar sin servidor, usa **Probar en modo demo** y no ingreses credenciales.

## Funciones

- Inicio de sesión y URL configurable del servidor.
- Conmutación entre control manual y navegación autónoma.
- Comandos direccionales, lecturas de sensores y estado de los cuatro LEDs.
- Mapa 2D actualizado mediante consulta periódica.
- Lista de canciones, reproducción, pausa, detención y volumen.
- Actualización de telemetría cada tres segundos y estado de conexión visible.

## API esperada

La app consulta `/cgi-bin/status`, `sensors`, `leds`, `map` y `audiolist`. Los `POST` envían JSON: login usa `username`/`password`, modo usa `mode`, y motores usa `direction`/`speed` (0–100). Las solicitudes autenticadas envían el token directamente en `Authorization`, sin prefijo `Bearer`, para coincidir también con los handlers de modo y mapa. Las respuestas usan `{ "status": "ok", "data": ... }`; el mapa del servidor expone `grid`, que el cliente normaliza a `cells`, y los sensores exponen `front_obstacle`/`side_obstacle`, que el cliente presenta como frontal/lateral.

La app está conectada a los endpoints actuales, pero los handlers del servidor en el repositorio aún devuelven respuestas de prueba: el login no valida credenciales y los comandos no accionan el hardware. Se requiere completar esos handlers y su integración con `librobot` antes de usarla para controlar el robot real. El mapa espera celdas `0` desconocida, `1` visitada, `2` obstáculo y `3` posición del robot.