# Subsistema de audio

## Arquitectura

`librobot-audio.so` encapsula ALSA y `libmpg123`. El servidor no debe incluir `asoundlib.h` ni abrir dispositivos ALSA. La biblioteca crea dos trabajadores: uno para reproduccion normal y otro para notificaciones. Cada trabajador decodifica MP3 en segundo plano y escribe PCM S16_LE a ALSA.

API publica:

- `audio_init(const char *device)`
- `audio_play(const char *path)`
- `audio_pause()`
- `audio_resume()`
- `audio_stop()`
- `audio_set_volume(unsigned int percent)`
- `audio_get_state()`
- `audio_play_notification(const char *path)`
- `audio_shutdown()`

El volumen se aplica digitalmente entre 0 y 100 por ciento. El dispositivo recomendado es `default`, configurado por ALSA en el target.

## Decision del decodificador

Se utiliza `libmpg123` porque ofrece una API C directa, bajo consumo y una integracion sencilla mediante `pkg-config`. No se añade PulseAudio, PipeWire ni un servidor multimedia. `mpg123` se empaqueta como dependencia Yocto y su licencia LGPL permite enlazarlo desde esta biblioteca respetando las obligaciones de redistribucion.

`libmad` tambien es viable y puede ser pequeño, pero su integracion y disponibilidad dependen mas de la version y las capas Yocto. `FFmpeg` ofrece mas formatos, pero aumenta notablemente el rootfs, RAM, tiempo de compilacion y superficie de mantenimiento. Para MP3 local en una Raspberry Pi 4, `libmpg123` es el compromiso mas sencillo.

## Compilacion en Ubuntu

Desde la raiz del repositorio:

```bash
sudo apt install build-essential cmake pkg-config libasound2-dev libmpg123-dev mpg123
cmake -S src/audio -B /tmp/robot-audio-build -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/robot-audio-build --parallel
ctest --test-dir /tmp/robot-audio-build --output-on-failure
```

Para una prueba con un MP3 local:

```bash
/tmp/robot-audio-build/audio-test /ruta/cancion.mp3 /ruta/notificacion.mp3
```

## Yocto

La receta es `src/meta-robot/recipes-robot/audio/robot-audio_1.0.bb` y declara:

```bitbake
DEPENDS = "alsa-lib mpg123"
RDEPENDS:${PN} = "alsa-lib" # libmpg123 se añade por la dependencia ELF
```

La imagen de ejemplo añade `robot-audio` mediante `IMAGE_INSTALL:append`. Desde un entorno Yocto inicializado:

```bash
bitbake-layers add-layer /ruta/Proyecto_1_Empotrados/src/meta-robot
bitbake-layers show-layers
bitbake-layers show-recipes robot-audio
bitbake example
```

Si la imagen se llama de otra manera, se debe sustituir `example` por el nombre real de la imagen.

El servicio persistente se inicia con systemd. Los CGI no enlazan ALSA: llaman a `/usr/bin/audioctl`, que usa el socket Unix `/run/robot-audio.sock`.

Ejemplos de control en el target:

```bash
systemctl status robot-audio
/usr/bin/audioctl play /usr/share/robot-audio/cancion.mp3
/usr/bin/audioctl pause
/usr/bin/audioctl resume
/usr/bin/audioctl volume 70
/usr/bin/audioctl stop
/usr/bin/audioctl notify /usr/share/robot-audio/obstacle.mp3
```

## QEMU y Raspberry Pi

QEMU permite validar compilacion de la receta, presencia de la biblioteca, dependencias, instalacion del binario y funcionamiento de la API con un dispositivo ALSA virtual si la maquina se lanza con audio configurado. No demuestra que el DAC, amplificador, niveles electricos, cableado ni el altavoz funcionen.

En Raspberry Pi 4 se debe seleccionar el backend ALSA adecuado (`bcm2835` para salida HDMI/analoga cuando corresponda, o el dispositivo USB/I2S que aparezca en `aplay -l`). Un DAC I2S requiere conectar BCLK, LRCLK, DATA y GND al conector GPIO siguiendo el pinout y habilitar el overlay del DAC. Un amplificador analogico debe recibir la salida del DAC o de una salida de audio adecuada; nunca se debe conectar un altavoz de potencia directamente a un GPIO.

## Evidencias

Guardar como minimo:

```bash
bitbake-layers show-layers
bitbake-layers show-recipes robot-audio
bitbake -e robot-audio | grep -E '^(DEPENDS|RDEPENDS):'
bitbake robot-audio -c compile -f
bitbake example
find tmp/deploy/images -type f -printf '%p %s bytes\n'
```

En el target:

```bash
ldconfig -p | grep robot-audio
aplay -l
/usr/bin/audio-test /ruta/cancion.mp3 /ruta/notificacion.mp3
ps -T -p $(pidof audio-test)
```

Para simultaneidad, ejecutar navegación, servidor y una prueba de audio al mismo tiempo y guardar:

```bash
top -b -n 1
free -h
systemctl status robot-server
```
