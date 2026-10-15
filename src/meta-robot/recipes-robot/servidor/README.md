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

## CAMBIOS IMPORTANTES:

Dentro de la carpeta de poky-scarthgap de cada uno, buscar este archivo

```bash
../poky-scarthgap-5.0.19/meta-openwrt/recipes-core/ustream-ssl/ustream-ssl_git.bb
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

Por último, en local.conf de rpi4 (o el que usen ustedes, build por ejemplo), añaden esto:

```bash
PACKAGECONFIG:pn-base-files = ""
```

Preferiblemente antes de IMAGE_INSTALL:append 