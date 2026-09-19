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
$ bitbake-layers show-layers
```

También pueden revisar en conf/bblayers.conf que estén añadidas. Ejemplo de mi propio archivo.
```bash
 /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-oe \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-webserver \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-python \
  /home/dell/poky-scarthgap-5.0.19/meta-openembedded/meta-networking \
  /home/dell/poky-scarthgap-5.0.19/meta-openwrt \
```

IMPORTANTE: Añadir estas dos líneas a conf/local.conf:
```bash
BBMASK += "/home/dell/poky-scarthgap-5.0.19/meta-openwrt/recipes-tweaks/packagegroups/packagegroup-core-boot.bbappend"
IMAGE_INSTALL:append = " uhttpd robot-server"
```
La ruta del BBmask debe ser acorde a cada uno de sus dispositivos. 