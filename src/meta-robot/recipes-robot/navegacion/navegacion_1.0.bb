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
inherit update-rc.d

do_install:append() {
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/app_robot.init \
        ${D}${sysconfdir}/init.d/app_robot
}

INITSCRIPT_NAME = "app_robot"
INITSCRIPT_PARAMS = "defaults 90"

FILES:${PN} += " \
    ${sysconfdir}/init.d/app_robot \
"
