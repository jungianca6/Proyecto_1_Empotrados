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
# Arquitectura del Sistema

### 1. Diagrama de arquitectura de Software
<img width="502" height="1280" alt="diagramasoftware" src="https://github.com/user-attachments/assets/2fcaa577-61ed-4f6e-8400-464c9e7926d9" />

### 2. Diagrama de arquitectura de Hardware

# Documentación de la API
### Pendiente

# Evidencias de compilación cruzada automática
### Pendiente

# Reporte de métricas
### Pendiente

# Resultados
### Pendiente 
