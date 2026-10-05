SUMMARY = "Aplicacion principal de navegacion del robot"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "librobot"
RDEPENDS:${PN} = "librobot"

SRC_URI = " \
    file://CMakeLists.txt \
    file://src/main.c \
    file://app_robot.init \
"

S = "${WORKDIR}"

inherit cmake

do_install:append() {
    # Instalar script de inicio
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/app_robot.init \
        ${D}${sysconfdir}/init.d/app_robot

    # Crear enlaces para arranque automatico
    install -d ${D}${sysconfdir}/rc3.d
    install -d ${D}${sysconfdir}/rc5.d

    ln -sf ../init.d/app_robot \
        ${D}${sysconfdir}/rc3.d/S90app_robot

    ln -sf ../init.d/app_robot \
        ${D}${sysconfdir}/rc5.d/S90app_robot
}

FILES:${PN} += " \
    ${sysconfdir}/init.d/app_robot \
    ${sysconfdir}/rc3.d/S90app_robot \
    ${sysconfdir}/rc5.d/S90app_robot \
"
