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
Pendiente por determinar de forma concreta


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

Asegúrese de incluir la capa personalizada del proyecto en su entorno de construcción:

``` bash
# Pendiente
```

### 1.2. Construcción de la Imagen y el SDK

Para generar la imagen mínima y el SDK de desarrollo (necesario para la compilación cruzada):

```bash
# Pendiente
```

## 2. Guía de Compilación Cruzada (Cross-Compilation)

Este procedimiento permite generar los binarios de la aplicación y la biblioteca dinámica utilizando el Toolchain de Yocto.

### 2.1. Preparar el Entorno (Environment Setup)

Es obligatorio exportar las variables de entorno para que el sistema utilice el compilador cruzado en lugar del nativo:

```bash
# Pendiente
```

### 2.2. Construcción del Proyecto 
```bash
cd ~/poky-scarthgap-5.0.15
source oe-init-build-env
bitbake core-image-minimal
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

# Documentación de la API
### Pendiente

# Evidencias de compilación cruzada automática
### Pendiente

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
### Pendiente 