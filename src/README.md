# Guía de Ejecución y Pruebas 

Para añadir el layer meta-robot al entorno de Yocto, para cada miembro del grupo.

## 1. Crear la imagen de Raspberry Pi 4 básica con Yocto
Los mismos pasos especificados por el profesor en el taller 4 y 5, para las máquinas QEMU y RaspberryPi4. 
Si ya se tienen creadas, ignorar este paso.

## 2. Localizar las carpetas del paso 1
Ejemplo:
```bash
cd poky-scarthgap-5.0.19
source oe-init-build-env [nombre de carpeta]
```
Nota: Siempre se debe realizar el segundo comando, cada vez que se reinicia la computadora, 
y se desea trabajar con funciones y métodos propios de Yocto, como bitbake-layers

## 3. Añadir el meta-layer meta-robot al entorno de Yocto
En este paso, se debe ejecutar el siguiente comando en la terminal, para añadir el layer meta-robot al entorno de Yocto:
```bash
bitbake-layers add-layer ~/Escritorio/Proyecto_1_Empotrados/src/meta-robot
```

## 4. Verificar que el layer fue añadido correctamente al entorno
Ejecutar este comando en la terminal:
```bash
bitbake-layers show-layers
```
Si aparece esto en el resultado, significa que el layer fue añadido correctamente al entorno de Yocto:
```bash
meta-robot            /home/dell/Escritorio/Proyecto_1_Empotrados/src/meta-robot  
```