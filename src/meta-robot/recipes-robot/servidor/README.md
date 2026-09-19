# Guía para el Servidor uhttpd 

Clonar el repositorio en el entorno de Yocto (en mi caso, /poky-scarthgap-5.0.19/rpi4, después de haber hecho source oe-init-build-env rpi4)

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
dell@dell-Inspiron-3585:~/poky-scarthgap-5.0.19/rpi4$ bitbake-layers show-layers
NOTE: Starting bitbake server...
layer                 path                                                                    priority
========================================================================================================
core                  /home/dell/poky-scarthgap-5.0.19/meta                                   5
yocto                 /home/dell/poky-scarthgap-5.0.19/meta-poky                              5
yoctobsp              /home/dell/poky-scarthgap-5.0.19/meta-yocto-bsp                         5
raspberrypi           /home/dell/poky-scarthgap-5.0.19/meta-raspberrypi                       9
meta-robot            /home/dell/Escritorio/Proyecto_1_Empotrados/src/meta-robot              6
openembedded-layer    /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-oe              5
webserver             /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-webserver       5
meta-python           /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-python          5
networking-layer      /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-networking      5
openwrt-layer         /home/dell/poky-scarthgap-5.0.19/meta-openwrt                           8
dell@dell-Inspiron-3585:~/poky-scarthgap-5.0.19/rpi4$ source oe-init-build-env rpi4
```

También pueden revisar en conf/local.conf que estén añadidas
```bash
 /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-oe \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-webserver \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-python \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-networking \
  /home/dell/poky-scarthgap-5.0.19/meta-openwrt \
```

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