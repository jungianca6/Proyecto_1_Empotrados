# Proyecto I. Sistema embebido a la medida para un robot aspiradora autónomo con reproducción de audio y control remoto

## 1. Información General
* **Institución:** Instituto Tecnológico de Costa Rica
* **Escuela:** Ingeniería en Computadores
* **Curso:** CE-1113 Sistemas Empotrados
* **Profesor:** Dr.-Ing. Jeferson González Gómez
* **Fecha de entrega:** 6 de octubre de 2026
* **Equipo:** Giancarlo Vega, Bryan Feng, Allison Redondo, Sebastian Chaves


## 2. Descripción del Proyecto
Desarrollo de un sistema embebido a medida para un robot aspiradora autónomo basado en la **Raspberry Pi 4**. El proyecto utiliza **Yocto Project** para crear una distribución Linux mínima y personalizada que integra navegación reactiva, reproducción de audio MP3 concurrente y una interfaz de control remoto (Web/App) con visualización de mapa 2D.


## 3. Estructura Organizativa (Roles)
Pendiente por determinar de forma concreta


## 4. Matriz de Requerimientos

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


## 5. Flujo de Trabajo en Git

### 5.1. Estrategia de Ramas
* `main`: Rama de producción (código estable).
* `development`: Rama de integración para el trabajo diario.
* `feat/nombre-tarea`: Desarrollo de nuevas funciones.
* `fix/nombre-error`: Corrección de fallos.

### 5.2. Convención de Commits (Conventional Commits)
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

### 5.3. Pull Requests
* Todo PR debe estar vinculado a un **Issue**.