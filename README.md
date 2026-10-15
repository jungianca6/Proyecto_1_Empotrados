# Proyecto I. Sistema embebido a la medida para un robot aspiradora autónomo con reproducción de audio y control remoto

## Información General
* **Institución:** Instituto Tecnológico de Costa Rica
* **Escuela:** Ingeniería en Computadores
* **Curso:** CE-1113 Sistemas Empotrados
* **Profesor:** Dr.-Ing. Jeferson González Gómez
* **Fecha de entrega:** 15 de octubre de 2026
* **Equipo:** Giancarlo Vega, Bryan Feng, Alisson Redondo, Sebastian Chaves


## Descripción del Proyecto
Desarrollo de un sistema embebido a medida para un robot aspiradora autónomo basado en la **Raspberry Pi 4**. El proyecto utiliza **Yocto Project** para crear una distribución Linux mínima y personalizada que integra navegación reactiva, reproducción de audio MP3 concurrente y una interfaz de control remoto (Web/App) con visualización de mapa 2D.


## Estructura Organizativa (Roles)
* **Giancarlo Vega:** Entorno de repositorio, receta del servidor, documentación
* **Bryan Feng:** CGI del uhttpd, API, mapeo primitivo
* **Alisson Redondo:** Hardware, navegación automática y manual, Wi-Fi de Raspberry Pi 4, métricas
* **Sebastian Chaves:** Aplicación móvil, documentación de atributos, audio primitivo. 


## Matriz de Requerimientos

| Código | Categoría | Requerimiento | Descripción Detallada |
| :--- | :--- | :--- | :--- |
| **RF-01** | Funcional | Navegación Autónoma | Implementar un algoritmo reactivo (aleatorio, espiral o zig-zag) para cubrir el área de limpieza y reaccionar ante obstáculos. |
| **RF-02** | Funcional | Detección de Obstáculos | Procesar en tiempo real señales de al menos dos sensores (ultrasónicos o infrarrojos) para evitar colisiones. | 
| **RF-03** | Funcional | Control de Tracción | Gestionar el movimiento diferencial (avance, retroceso, giros) mediante señales PWM para variar la velocidad. | 
| **RF-04** | Funcional | Reproducción MP3 | Reproducir archivos de audio locales de forma concurrente con el movimiento del robot. |
| **RF-05** | Funcional | Retroalimentación Sonora | Emitir audios cortos ante los eventos de inicio del sistema, inicio del modo autónomo, detección de obstáculos y cambio a modo manual. | 
| **RF-06** | Funcional | Mapeo 2D | Construir una grilla incremental que identifique zonas visitadas, desconocidas y obstáculos detectados. |
| **RF-07** | Funcional | Control Remoto Manual | Permitir el mando directo de los motores desde la interfaz web (flechas de dirección). | 
| **RF-08** | Funcional | Modos de Operación | Permitir cambiar entre modo autónomo y modo manual desde la interfaz de control. |
| **RF-09** | Funcional | Salida de Audio | Reproducir el audio directamente desde el robot mediante un parlante conectado al sistema embebido. |
| **RA-01** | Arquitectura y OS | Imagen Yocto Mínima | Construir una distribución Linux optimizada que incluya solo los paquetes necesarios (servidor web, bibliotecas de audio). |
| **RA-02** | Arquitectura y OS | Biblioteca Dinámica | Crear una .so que encapsule el acceso a GPIO (sensores/LEDs), PWM (motores) y audio, siendo utilizada exclusivamente por el servidor para interactuar con el hardware. |
| **RA-03** | Arquitectura y OS | Receta BitBake (.bb) | Desarrollar una receta propia para integrar el software del proyecto en la imagen Yocto de forma automatizada y reproducible. | 
| **RA-04** | Arquitectura y OS | Compilación Cruzada | Utilizar un toolchain ARM y sistema CMake/Autotools para compilar desde el host hacia la Raspberry Pi 4. | 
| **RA-05** | Arquitectura y OS | Aislamiento Galvánico | Implementar aislamiento galvánico u óptico entre la lógica de control de la Raspberry Pi y la etapa de potencia de los motores.  | 
| **RA-06** | Arquitectura y OS | Inicio Automático | Configurar una unidad systemd propia para iniciar automáticamente el servidor al energizar la Raspberry Pi y reiniciarlo ante fallos. |
| **RI-01** | Interfaz de Usuario | Panel de Control Web | Visualizar en tiempo real: modo activo, estado de sensores, estado de LEDs y el mapa de recorrido. | 
| **RI-02** | Interfaz de Usuario | Gestión de Audio | Controles para seleccionar canciones de una lista, reproducir, pausar, detener y ajustar volumen. | 
| **RI-03** | Interfaz de Usuario | Sistema de Login | Proporcionar una pantalla inicial de inicio de sesión con al menos un usuario registrado, evitando el almacenamiento de contraseñas en texto plano| 
| **RI-04** | Interfaz de Usuario | Indicadores Físicos | Gestión de 4 LEDs físicos: Modo Autónomo, Modo Manual, Alerta de Obstáculo y Sistema Encendido. |
| **RI-05** | Interfaz de Usuario | Comunicación Inalámbrica | Permitir la comunicación entre el cliente PC/móvil y la Raspberry Pi mediante conectividad inalámbrica, utilizando Wi-Fi como medio principal. | 
| **RNF-01** | No Funcionales | Flujo Git Profesional | Uso de Conventional Commits y ramas (main, develop, feat) para el control de versiones. |
| **RH-01** | Hardware | Modelo Físico | Construir un modelo físico de robot aspiradora con chasis, dos motores DC con controladores, rueda loca, sensores de proximidad, LEDs, salida de audio y alimentación portátil. |
| **RH-02** | Hardware | Circuitería de Control | Implementar las conexiones GPIO, circuitos de potencia de motores, aislamiento entre lógica y potencia y acondicionamiento de las señales de los sensores. |
| **RNF-02** | No Funcionales | Seguridad de Potencia | Utilizar un sistema de alimentación regulado y protegido mediante BMS, manteniendo rieles independientes para lógica y motores.  |
| **RNF-03** | No Funcionales | Documentación Técnica | Mantener un README completo con instalación, compilación cruzada, generación de imagen Yocto, receta propia, configuración y uso del sistema, diagramas de arquitectura, documentación de la biblioteca, evidencias de ejecución y métricas.  |
| **RNF-04** | No Funcionales | Atributos Provisionales | Redacción de los documentos de Diseño (DI) y Aprendizaje Continuo (AC) según indicadores del TEC. |
| **RNF-05** | No Funcionales | Métricas de Eficiencia | Medir y reportar el tamaño del rootfs, tiempo de arranque y uso de RAM/CPU durante la operación simultánea de navegación autónoma, audio y servidor web. |
| | | | | |


## Flujo de Trabajo en Git

### 1. Estrategia de Ramas
* `main`: Rama de producción (código estable).
* `development`: Rama de integración para el trabajo diario.
* `feat/nombre-tarea`: Desarrollo de nuevas funciones.
* `fix/nombre-error`: Corrección de fallos.

### 2. Convención de Commits (Conventional Commits)
Formato: `tipo(alcance): descripción`
* `feat`: Nueva funcionalidad.
* `fix`: Corrección de errores.
* `docs`: Cambios en README o documentación de atributos (DI/AC).
* `refactor`: Mejoras en código existente sin cambiar comportamiento.
* `chore`: Mantenimiento general del proyecto.
* `test`: Creación o actualización de pruebas.
* `ci`: Cambios en flujos de CI/CD.
* `build`: Cambios de compilación, toolchain o sistema de build.
* `perf`: Optimización de rendimiento.
* `style`: Cambios de estilo/formato sin impacto funcional.

### 3. Pull Requests
* Todo PR debe estar vinculado a un **Issue**.

## Aplicación móvil

La app Android/iOS está en [`mobile/`](mobile/README.md) y utiliza React Native con Expo. Para iniciarla en una estación con Node.js LTS:

```bash
cd mobile
npm install
npm start
```

La app ofrece inicio de sesión, control manual/autónomo, lecturas de sensores, mapa 2D, estado de LEDs y controles de audio. Para usarla con el robot, configura la URL del servidor de la Raspberry Pi en la pantalla de acceso. Los endpoints CGI actuales son prototipos y deben integrarse con `librobot` antes de controlar hardware real.

# Guía Técnica y de Compilación

Este documento detalla los procedimientos de configuración, generación de imagen y compilación cruzada para el sistema operativo personalizado del Robot Aspiradora. El sistema está optimizado para arquitecturas **ARM Cortex-A72/A76** (Raspberry Pi 4).

## 1. Generación de la Imagen con Yocto Project 

Para replicar la distribución Linux completa, siga los pasos de configuración del entorno de BitBake.

### 1.1. Configuración de Capas

En este paso, se debe ejecutar el siguiente comando en la terminal, para añadir el layer meta-robot al entorno de Yocto:
```bash
bitbake-layers add-layer ~/Escritorio/Proyecto_1_Empotrados/src/meta-robot
```

Ejecutar este comando en la terminal:
```bash
bitbake-layers show-layers
```
Si aparece esto en el resultado, significa que el layer fue añadido correctamente al entorno de Yocto:
```bash
meta-robot            /home/dell/Escritorio/Proyecto_1_Empotrados/src/meta-robot  
```

### 1.2. Configuración de Servidor

Clonar el repositorio en el entorno de Yocto:

```bash
git clone -b scarthgap https://git.openembedded.org/meta-openembedded
```

Añadir las layers necesarias 
```bash
$ bitbake-layers add-layer ../meta-openembedded/meta-oe
$ bitbake-layers add-layer ../meta-openembedded/meta-python
$ bitbake-layers add-layer ../meta-openembedded/meta-networking
$ bitbake-layers add-layer ../meta-openwrt
```
Revisar que fueron añadidas al entorno de Yocto
```bash
$ bitbake-layers show-layers
```

### CAMBIOS IMPORTANTES:

Dentro de la carpeta de poky-scarthgap de cada uno, buscar este archivo

```bash
../poky-scarthgap-5.0.1X/meta-openwrt/recipes-core/ustream-ssl/ustream-ssl_git.bb
```

Busquen el "do_install:append()", y lo cambian completo por esto:

```bash
do_install:append() {
	install -d ${D}${includedir}/libubox
	install -m 0644 ${S}/*.h ${D}${includedir}/libubox

	if [ "${base_libdir}" != "${libdir}" ]; then
		install -dm 0755 ${D}${base_libdir}
		mv ${D}${libdir}/libustream-ssl.so ${D}${base_libdir}/libustream-ssl.so
		rmdir --ignore-fail-on-non-empty ${D}${libdir}
	fi
}
```

### 1.3. Archivos locales
Asegurarse que conf/local.conf en la máquina local se vea asi:

```bash
# Configuración del objetivo de hardware
MACHINE = "raspberrypi4-64"

# Servidor SSH para acceso remoto
EXTRA_IMAGE_FEATURES += "ssh-server-dropbear"

# Control de hilos para no agotar la memoria RAM
BB_NUMBER_THREADS = "4"
PARALLEL_MAKE = "-j 4"

# Limpieza automática de temporales por paquete
INHERIT += "rm_work"

# Máscara de recetas
BBMASK += "/home/alissonrm/poky-scarthgap-5.0.15/meta-openwrt/recipes-tweaks/packagegroups/packagegroup-core-boot.bbappend"

# Aplicaciones del proyecto y controladores de Wi-Fi
IMAGE_INSTALL:append = " \
    kernel-modules \
    librobot \
    navegacion \
    wpa-supplicant \
    linux-firmware-bcm43455 \
    wireless-regdb-static \
    uhttpd \
    robot-server \
    robot-wifi \
    robot-metrics  \
"

# Configuración de contraseña para el usuario root
INHERIT += "extrausers"

EXTRA_USERS_PARAMS = " \
    usermod -p '$6$POKhlKP0qRF15xV.$fRD/74BemCUY1bfxgP8PpUyP8Fro6D/G0Pww1E/nBNDG8DgiGHbZOW/4YNriKjsZ7RSUaezTzA6zUjDSfO2Xa0' root; \
"

# Mantener systemd como gestor de inicio
INIT_MANAGER = "systemd"

ENABLE_AUDIO = "1"


ERROR_QA:remove = "usrmerge"
WARN_QA:append = " usrmerge"

BAD_RECOMMENDATIONS += "os-release"

LICENSE_FLAGS_ACCEPTED += "synaptics-killswitch"


PACKAGECONFIG:append:pn-mpg123 = " alsa"

KERNEL_MODULE_AUTOLOAD += "snd_bcm2835"
KERNEL_MODULE_PROBECONF += "snd_bcm2835"
module_conf_snd_bcm2835 = "options snd_bcm2835 enable_headphones=1 enable_hdmi=0"
```

### 1.3. Construcción de la Imagen y el SDK

Para generar la imagen mínima y el SDK de desarrollo (necesario para la compilación cruzada):

```bash
cd ~/poky-scarthgap-5.0.15
source oe-init-build-env
bitbake core-image-minimal
```
Crear el toolchain:
```bash
bitbake core-image-minimal -c populate_sdk
```
Instalar el toolchain:
```bash
. tmp/deploy/sdk/poky-glibc-x86_64-meta-toolchain-cortexa72-poky-linux-toolchain-5.0.15.sh
```
Esto se hace solo una vez. 
## 2. Guía de Compilación Cruzada (Cross-Compilation)

Este procedimiento permite generar los binarios de la aplicación y la biblioteca dinámica utilizando el Toolchain de Yocto.

### 2.1. Construcción del Proyecto 
```bash
ls -lt ~/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/*wic* | head
lsblk
cd ~/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64
bunzip2 -k core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic.bz2
sudo umount /dev/sda1 2>/dev/null || true
sudo umount /dev/sda2 2>/dev/null || true
sudo dd if=core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic \
  of=/dev/sda \
  bs=4M \
  status=progress \
  conv=fsync
sync
sudo partprobe /dev/sda
```

# Arquitectura del Sistema

### 1. Diagrama de arquitectura de Software
<img width="502" height="1280" alt="diagramasoftware" src="https://github.com/user-attachments/assets/2fcaa577-61ed-4f6e-8400-464c9e7926d9" />

### 2. Diagrama de arquitectura de Hardware
<img width="1536" height="794" alt="arquitectura" src="https://github.com/user-attachments/assets/efb8c158-c8db-4527-804e-3d616ee69fac" />

# Documentación de la API
## Descripción general

El proyecto implementa una biblioteca dinámica denominada `librobot.so`, desarrollada en C, que proporciona una API pública para controlar y consultar los componentes del robot aspiradora.

Su objetivo es encapsular el acceso al hardware y ofrecer una interfaz común para los distintos módulos del sistema, evitando que cada componente tenga que interactuar directamente con los controladores de bajo nivel.

La interfaz pública se define en `librobot.h` y su implementación se encuentra en `librobot.c`. Estos archivos actúan como una capa de abstracción sobre los módulos internos de motores, sensores, LEDs, cepillos, motor de aspiración y navegación.

La biblioteca es utilizada por la aplicación de navegación autónoma y por los programas CGI del servidor web `uhttpd`, permitiendo que las operaciones solicitadas desde la interfaz web se comuniquen con los componentes del robot a través de una API común.

## Arquitectura de la biblioteca

La arquitectura de comunicación se organiza de la siguiente manera:

1. **Interfaz web:** permite al usuario interactuar con el robot desde un navegador.
2. **Servidor web `uhttpd`:** recibe las solicitudes HTTP y ejecuta los programas CGI correspondientes.
3. **Programas CGI:** procesan las solicitudes y utilizan las funciones públicas de `librobot.so` para ejecutar las operaciones solicitadas.
4. **Biblioteca dinámica `librobot.so`:** expone una interfaz común y delega las operaciones a los módulos correspondientes.
5. **Módulos internos:** implementan el acceso al hardware y las funcionalidades de motores, sensores, LEDs, cepillos, aspiración y navegación autónoma.

La aplicación de navegación también utiliza la biblioteca para acceder a los componentes del robot. De esta forma, tanto la navegación autónoma como el servidor web pueden utilizar una interfaz compartida.

Los archivos principales relacionados con esta arquitectura son:

| Archivo o componente | Responsabilidad |
|---|---|
| `librobot.h` | Declaración de la API pública y los tipos expuestos por la biblioteca. |
| `librobot.c` | Implementación de las funciones públicas y delegación hacia los módulos internos. |
| `motors.c` y `motors.h` | Control de los movimientos del robot. |
| `sensors.c` y `sensors.h` | Lectura de los sensores de obstáculos. |
| `leds.c` y `leds.h` | Control y consulta de los LEDs indicadores. |
| `brushes.c` y `brushes.h` | Activación y desactivación de los cepillos. |
| `vacuum.c` y `vacuum.h` | Activación y desactivación del motor de aspiración. |
| Módulos de navegación y `fsm.h` | Implementación y consulta del estado de la máquina de estados finitos (FSM). |
| Programas CGI | Comunicación entre las solicitudes HTTP y las funciones de la biblioteca. |

## Convenciones de la API

Las funciones públicas utilizan valores enteros como resultado de sus operaciones.

En general, las funciones de inicialización devuelven el resultado del módulo interno correspondiente. Las funciones de control que actualmente no propagan errores de los módulos de hardware devuelven `0` después de invocar la operación.

Por convención, las funciones que consultan sensores devuelven valores que representan el estado detectado o un error. Las funciones de consulta de LEDs y de navegación devuelven el valor correspondiente al estado solicitado.

**Importante:** un retorno de `0` indica que la función pública completó su ejecución según su implementación actual, pero no necesariamente garantiza que el hardware haya realizado correctamente la operación. Algunas funciones todavía no verifican ni propagan los errores del módulo subyacente.

## API de motores

La API de motores permite inicializar y liberar los recursos asociados al control del movimiento, ejecutar desplazamientos y detener el robot.

La implementación actual controla el movimiento del robot completo, en lugar de ofrecer control independiente sobre cada motor.

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_motor_init()` | Ninguno | Resultado de `motors_init()` | Inicializa el subsistema de motores. |
| `robot_motor_deinit()` | Ninguno | `0` | Libera los recursos del subsistema de motores. |
| `robot_motor_move(direction, speed_pct)` | Dirección y velocidad solicitada | `0` en la implementación actual | Ejecuta un movimiento del robot. |
| `robot_motor_stop_all()` | Ninguno | `0` en la implementación actual | Solicita detener el movimiento del robot. |

El parámetro `direction` utiliza el tipo `RobotDirection`, definido por el módulo de motores. La dirección `ROBOT_STOP` representa la orden de detener el movimiento.

El parámetro `speed_pct` representa la velocidad solicitada en porcentaje:

- `0`: robot detenido.
- `1–100`: velocidad solicitada.

La implementación de `robot_motor_move()` limita el valor recibido al intervalo de 0 a 100. Sin embargo, el control PWM todavía no está implementado: el parámetro se recibe y valida, pero el código actual activa los motores a plena potencia cuando corresponde. Por lo tanto, los valores intermedios no garantizan velocidades físicas proporcionales.

## API de sensores

La API de sensores permite inicializar el subsistema y consultar la presencia de obstáculos.

Los sensores FC-51 utilizados en la implementación actual proporcionan señales digitales. Por ello, permiten determinar si se detecta un obstáculo, pero no medir su distancia en centímetros.

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_sensor_init()` | Ninguno | Resultado de `sensors_init()` | Inicializa los sensores. |
| `robot_sensor_deinit()` | Ninguno | `0` | Finaliza la operación de la API de sensores. Actualmente no libera recursos adicionales. |
| `robot_sensor_front_obstacle()` | Ninguno | `1`, `0` o `-1` | Consulta la detección de obstáculos frontales. |
| `robot_sensor_side_obstacle()` | Ninguno | Según la implementación del sensor | Consulta la detección de obstáculos laterales. |

Los valores documentados para la consulta frontal son:

- `1`: obstáculo detectado.
- `0`: camino despejado.
- `-1`: error.

La función `robot_sensor_side_obstacle()` delega la consulta en `sensor_side_obstacle_detected()`. Su interpretación exacta depende de los valores y las condiciones de error definidos en la implementación de ese módulo.

## API de LEDs

La API de LEDs permite inicializar el subsistema, liberar sus recursos, modificar el estado de un indicador y consultar su estado actual.

Los cuatro indicadores se identifican mediante el tipo enumerado `robot_led_t`.

| Identificador | Valor | Propósito |
|---|---:|---|
| `LED_AUTO` | `0` | Indicador del modo autónomo. |
| `LED_MANUAL` | `1` | Indicador del modo manual. |
| `LED_OBSTACLE` | `2` | Indicador de detección de obstáculos. |
| `LED_POWER` | `3` | Indicador del sistema o alimentación. |

Las funciones disponibles son:

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_led_init()` | Ninguno | Resultado de `leds_init()` | Inicializa el subsistema de LEDs. |
| `robot_led_deinit()` | Ninguno | `0` | Libera los recursos del subsistema. |
| `robot_led_set(led, state)` | Identificador y estado solicitado | `0` si el identificador es válido; `-1` si no lo es | Solicita cambiar el estado de un LED. |
| `robot_led_get(led)` | Identificador del LED | Estado devuelto por el módulo o `-1` si el identificador es inválido | Consulta el estado de un LED. |

El parámetro `state` se transmite a la función específica de cada indicador. Su interpretación, por ejemplo, como encendido o apagado, depende de la implementación del módulo de LEDs.

Actualmente, la API está definida e integrada en `librobot.c`. Su disponibilidad funcional depende de la implementación y validación del módulo de hardware correspondiente.

## API de cepillos

La API de cepillos permite controlar el mecanismo de limpieza sin intervenir directamente en el subsistema de movimiento.

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_brushes_on()` | Ninguno | `0` en la implementación actual | Solicita activar los cepillos. |
| `robot_brushes_off()` | Ninguno | `0` en la implementación actual | Solicita desactivar los cepillos. |

Ambas funciones delegan la operación en `brushes_on()` y `brushes_off()`, respectivamente.

Esta separación permite controlar los cepillos independientemente del movimiento del robot. Por ejemplo, el robot puede continuar desplazándose mientras los cepillos están apagados, siempre que el resto de la lógica de control lo permita.

## API del motor de aspiración

La API del motor de aspiración permite activar y desactivar el mecanismo encargado de aspirar la suciedad.

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_vacuum_on()` | Ninguno | `0` en la implementación actual | Solicita activar el motor de aspiración. |
| `robot_vacuum_off()` | Ninguno | `0` en la implementación actual | Solicita desactivar el motor de aspiración. |

Estas funciones delegan las operaciones en `vacuum_on()` y `vacuum_off()`.

Al igual que ocurre con los cepillos, el control del motor de aspiración está separado de la API de movimiento. Esto permite gestionar los mecanismos de limpieza sin tener que detener necesariamente la navegación.

Los valores de retorno actuales no confirman por sí solos que el motor físico haya respondido correctamente.

## API de audio

La API de audio define una interfaz pública para inicializar el subsistema, reproducir archivos, pausar y detener la reproducción, modificar el volumen y liberar los recursos utilizados.

| Función | Parámetros | Retorno actual | Descripción |
|---|---|---|---|
| `robot_audio_init()` | Ninguno | `-1` | Inicialización del subsistema de audio. |
| `robot_audio_deinit()` | Ninguno | `0` | Finalización de la API de audio. |
| `robot_audio_play(filepath)` | Ruta del archivo de audio | `-1` | Solicitud de reproducción de un archivo. |
| `robot_audio_pause()` | Ninguno | `-1` | Solicitud de pausa de la reproducción. |
| `robot_audio_stop()` | Ninguno | `-1` | Solicitud de detener la reproducción. |
| `robot_audio_set_volume(volume)` | Volumen solicitado | `-1` | Solicitud de modificación del volumen. |

Aunque estas funciones forman parte de la interfaz pública, la implementación actual en `librobot.c` todavía no incorpora la funcionalidad de audio: las operaciones principales devuelven `-1`.

Por tanto, esta sección documenta la interfaz prevista, no una funcionalidad que pueda considerarse operativa. La reproducción real requiere integrar el módulo de audio y sustituir estas implementaciones provisionales.

## API de navegación

La navegación autónoma utiliza una máquina de estados finitos (FSM) para representar el estado del sistema de navegación.

La biblioteca expone una función para consultar dicho estado:

| Función | Parámetros | Retorno | Descripción |
|---|---|---|---|
| `robot_navigation_state()` | Ninguno | Estado devuelto por `fsm_get_state()`, convertido a entero | Consulta el estado actual de la máquina de estados de navegación. |

La función delega la consulta en `fsm_get_state()`. La interpretación del valor devuelto depende de los estados definidos en la implementación de la FSM.

Esta interfaz permite que otros componentes, como los programas CGI del servidor web, puedan consultar el estado de navegación sin acceder directamente a las estructuras internas de la máquina de estados.

## Integración con el servidor web

El servidor web `uhttpd` permite acceder a las funcionalidades del robot desde una interfaz web. Para ello, los programas CGI actúan como intermediarios entre las solicitudes HTTP y la biblioteca dinámica.

El flujo general de una operación es el siguiente:

1. El usuario ejecuta una acción desde la interfaz web.
2. El navegador envía una solicitud HTTP al servidor `uhttpd`.
3. El servidor ejecuta el programa CGI correspondiente.
4. El programa CGI procesa los parámetros de la solicitud y utiliza las funciones públicas de `librobot.so`.
5. La biblioteca delega la operación al módulo interno correspondiente.
6. El programa CGI devuelve una respuesta HTTP al navegador.

Este diseño separa la interfaz de usuario, la lógica de comunicación y el acceso al hardware.

Por ejemplo, una solicitud de activación de los cepillos puede procesarse mediante un programa CGI que invoque `robot_brushes_on()`. De forma similar, una solicitud de consulta de sensores puede utilizar `robot_sensor_front_obstacle()` para obtener la información necesaria para construir la respuesta al cliente.


# Evidencias de compilación cruzada automática
```bash
alissonrm@alissonrm-Vivobook-ASUSLaptop-X1504ZA-X1504ZA:~/poky-scarthgap-5.0.15/build$ ls -lt ~/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/*wic* | head
lrwxrwxrwx 1 alissonrm alissonrm        64 Oct 15 01:52 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs.wic.bz2 -> core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic.bz2
lrwxrwxrwx 1 alissonrm alissonrm        65 Oct 15 01:52 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs.wic.bmap -> core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic.bmap
-rw-r--r-- 1 alissonrm alissonrm      3812 Oct 15 01:52 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic.bmap
-rw-r--r-- 1 alissonrm alissonrm  72052510 Oct 15 01:52 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261015075056.wic.bz2
-rw-r--r-- 1 alissonrm alissonrm 382520320 Oct 15 00:54 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261015065312.wic
-rw-r--r-- 1 alissonrm alissonrm 393424896 Oct 14 14:42 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261014204032.wic
-rw-r--r-- 1 alissonrm alissonrm 393424896 Oct 14 13:44 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261014194224.wic
-rw-r--r-- 1 alissonrm alissonrm 393424896 Oct 14 11:58 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261014175707.wic
-rw-r--r-- 1 alissonrm alissonrm 393424896 Oct 14 10:51 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261014164832.wic
-rw-r--r-- 1 alissonrm alissonrm 393424896 Oct 14 10:27 /home/alissonrm/poky-scarthgap-5.0.15/build/tmp/deploy/images/raspberrypi4-64/core-image-minimal-raspberrypi4-64.rootfs-20261014162447.wic
alissonrm@alissonrm-Vivobook-ASUSLaptop-X1504ZA-X1504ZA:~/poky-scarthgap-5.0.15/build$ 
```

# Reporte de métricas

## Objetivo

Con el propósito de evaluar la eficiencia de los recursos de la imagen Linux
desarrollada para la Raspberry Pi 4, se implementó un mecanismo de recolección
automática de métricas durante el arranque del sistema.

El recolector registra información sobre el espacio utilizado por el sistema
de archivos raíz (rootfs), el tiempo transcurrido desde el arranque del kernel,
el consumo de memoria RAM y el porcentaje de utilización de CPU. 

La implementación utiliza un script de shell que obtiene los datos directamente
de las interfaces de Linux, principalmente `/proc` y las herramientas `df` y
`systemctl`. Los resultados se almacenan en un archivo JSON para facilitar su
consulta y su posible integración con el servidor web del robot.

## Herramientas utilizadas

| Herramienta o interfaz | Propósito |
| --- | --- |
| `/proc/uptime` | Obtener el tiempo transcurrido desde el arranque del kernel. |
| `df -kP /` | Consultar el tamaño, el espacio utilizado y el espacio disponible del sistema de archivos raíz. |
| `/proc/meminfo` | Obtener información sobre la memoria RAM total y disponible. |
| `/proc/stat` | Obtener los contadores de tiempo de CPU necesarios para calcular su utilización. |
| `/proc/<PID>/status` | Consultar la memoria residente de procesos específicos. |
| `pidof` | Comprobar la presencia de los procesos de navegación y del servidor web. |
| `systemctl` | Consultar el estado de los servicios del robot. |
| `awk` | Procesar los datos y convertir las unidades cuando corresponde. |
| `sleep` | Establecer los intervalos de estabilización y muestreo. |

Estas herramientas permiten obtener las métricas mediante interfaces estándar
de Linux, sin depender de aplicaciones gráficas adicionales.

## Automatización de la recolección

La recolección se implementa mediante el script `robot-metrics-collect`,
instalado en `/usr/sbin/robot-metrics-collect`, y el servicio
`robot-metrics.service`.

El servicio se configura como una unidad de tipo `oneshot` de systemd y se
habilita para ejecutarse durante el arranque. Su configuración establece
dependencias respecto a `robot-server.service` y `robot-wifi.service`.

Durante la ejecución, el recolector realiza las siguientes operaciones:

1. Crea el directorio destinado a almacenar los resultados.
2. Obtiene el tiempo transcurrido desde el arranque del kernel y consulta el
   estado de los servicios del servidor web y de la conexión Wi-Fi.
3. Consulta el espacio total, utilizado y disponible del sistema de archivos
   raíz.
4. Espera hasta 30 segundos para detectar el proceso `app_robot`, utilizado
   como indicador de que la navegación está activa.
5. Si detecta la navegación, espera 10 segundos adicionales para reducir la
   influencia del pico inicial de actividad.
6. Registra el consumo de memoria RAM del sistema y la memoria residente de
   los procesos de navegación y del servidor web, cuando están disponibles.
7. Calcula el porcentaje de utilización de CPU durante una ventana de
   muestreo de 5 segundos.
8. Genera el archivo JSON con los resultados y lo publica en la ubicación
   configurada.

El archivo se escribe inicialmente con el nombre temporal `metrics.json.tmp`
y posteriormente se renombra a `metrics.json`. Esto evita que un lector
encuentre el archivo definitivo mientras todavía se está escribiendo.

La ubicación del reporte es:

`/var/lib/robot/metrics.json`

## Métricas recopiladas

### 1. Tamaño del sistema de archivos raíz (rootfs)

El script utiliza el comando `df -kP /` para consultar la capacidad del sistema
de archivos que contiene la raíz `/`.

Los resultados se expresan en MiB, calculados a partir de los valores
reportados en KiB.

Se registran los siguientes valores:

- `rootfs_total_mb`: capacidad total del sistema de archivos raíz.
- `rootfs_used_mb`: espacio utilizado.
- `rootfs_available_mb`: espacio disponible.
- `rootfs_limit_mb`: presupuesto de referencia de 200 MB.

Esta medición describe el sistema de archivos raíz en el dispositivo después
del despliegue. No representa necesariamente el tamaño del archivo de imagen
generado por Yocto, ya que ambos valores dependen de aspectos diferentes del
proceso de construcción y del almacenamiento.

Para evaluar el cumplimiento del presupuesto, se debe indicar explícitamente
qué valor se está comparando con el límite de referencia y documentar el
método de medición utilizado.

### 2. Tiempo de arranque

El script consulta `/proc/uptime` para obtener el tiempo transcurrido desde el
arranque del kernel.

El valor `boot_seconds` corresponde al tiempo registrado cuando se ejecuta
el recolector. También se incluye `navigation_ready_seconds`, que registra
el tiempo transcurrido desde el arranque del kernel hasta que el script detecta
el proceso `app_robot`.

El presupuesto de referencia establecido por el proyecto es de 15 segundos.

Es importante señalar que estas mediciones no identifican directamente el
instante en que el servicio de control queda operativo. El tiempo de
`navigation_ready_seconds` depende de la detección del proceso de navegación,
mientras que la disponibilidad real del control también depende de que el
servidor web pueda recibir y procesar solicitudes.

Por ello, para verificar rigurosamente el requisito del proyecto, debe medirse
el tiempo desde el reset hasta una señal verificable de disponibilidad del
servicio de control, por ejemplo, una petición HTTP de prueba que reciba una
respuesta satisfactoria.

### 3. Uso de memoria RAM

La información de memoria se obtiene de `/proc/meminfo`.

El script utiliza `MemTotal` para determinar la memoria total reconocida por
el sistema y `MemAvailable` para estimar la memoria que puede utilizarse sin
recurrir excesivamente a intercambio de memoria.

La memoria utilizada se calcula como:

`RAM utilizada = MemTotal - MemAvailable`

Se registran los siguientes campos:

- `ram_total_mb`: memoria RAM total reconocida por Linux.
- `ram_available_mb`: memoria estimada como disponible.
- `ram_used_mb`: memoria calculada como total menos disponible.

Además, el recolector consulta el campo `VmRSS` de `/proc/<PID>/status` para
obtener la memoria residente de los siguientes procesos, cuando existen:

- `navigation_rss_kb`: memoria residente del proceso `app_robot`.
- `server_rss_kb`: memoria residente del proceso `uhttpd`.

La memoria residente representa la memoria física actualmente asociada a un
proceso. No debe interpretarse como una medida independiente del consumo total
del sistema, ya que la suma de la memoria residente de varios procesos puede
incluir páginas compartidas.

### 4. Uso de CPU

La utilización de CPU se calcula a partir de los contadores globales de
`/proc/stat`.

El recolector obtiene una primera muestra de los contadores, espera cinco
segundos y obtiene una segunda muestra. A partir de las diferencias entre
ambas muestras, calcula el porcentaje de tiempo que la CPU no estuvo inactiva.

La fórmula utilizada es:

`CPU (%) = 100 × (Δtiempo_total - Δtiempo_inactivo) / Δtiempo_total`

El cálculo considera como inactivo el tiempo reportado en los campos `idle`
e `iowait`.

El resultado se registra en `cpu_percent`.

Esta medición corresponde a la utilización global de la CPU durante el
intervalo de muestreo. No representa exclusivamente el consumo de la
navegación, del servidor web o del sistema de audio.

Para que la medición sea representativa de la operación normal, el robot debe
estar ejecutando simultáneamente la navegación autónoma, la reproducción de
audio y el servidor web. También debe documentarse si la medición se realizó
mientras el robot estaba moviéndose, evitando obstáculos y atendiendo
solicitudes desde un cliente.

### 5. Estado de los servicios

El reporte incluye indicadores para facilitar la interpretación de las
mediciones:

- `server_active`: indica si `robot-server.service` se encuentra activo
  cuando se consulta su estado.
- `wifi_active`: indica si `robot-wifi.service` se encuentra activo.
- `navigation_active`: indica si se detectó el proceso `app_robot`.

Estos indicadores ayudan a identificar si la medición se realizó con los
componentes principales presentes. Sin embargo, un servicio activo o un
proceso existente no garantizan por sí solos que toda su funcionalidad esté
operativa.

## Archivo de resultados

Los resultados se almacenan en formato JSON en:

`/var/lib/robot/metrics.json`

El archivo contiene los campos descritos anteriormente, junto con los límites
de referencia y los intervalos de muestreo utilizados.

Debido a que no se pueden realizar consultas por SSH a la Raspberry Pi 4, ni se tiene un cable micro HDMI para conectarlo a un monitor para ejecutar comandos directamente a la Raspberry, se envían los datos relevantes a la aplicación móvil, por medio de un CGI en `src/meta-robot/recipes-robot/servidor/files/cgi/metrics.c`. Se envía el tamaño de rootfs, el tiempo de arranque, y valores estáticos generales del uso de CPU y la memoria RAM.

## Resultados de las métricas

Las métricas recopiladas por la aplicación permiten evaluar el tamaño del sistema de archivos raíz, el tiempo de arranque y el consumo general de memoria RAM y CPU.

| Métrica | Valor obtenido | Unidad | Presupuesto de referencia |
|---|---:|---|---|
| Tamaño total del rootfs | 108.19 | MB | ≤ 200 MB |
| Tiempo de arranque | 15.23 | s | ≤ 15 s |
| Memoria RAM utilizada | 108.41 | MB | Informativo |
| Memoria RAM disponible | 1736.89 | MB | Informativo |
| Uso de CPU | 0.15 | % | Informativo |

# Resultados
<img width="1536" height="864" alt="5690bdcc-18a3-4f62-9778-d98c67425713" src="https://github.com/user-attachments/assets/48800842-6ed9-4708-bf26-d62e50aeca3e" />

<img width="1536" height="864" alt="67864bb9-be75-4188-a800-e2dcb39fc958" src="https://github.com/user-attachments/assets/658e3c04-af5d-4465-8029-339db075bcc4" />



