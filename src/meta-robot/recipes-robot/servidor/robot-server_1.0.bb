SUMMARY = "Servidor web de control del robot"
DESCRIPTION = "Servidor HTTP minimo para el panel remoto del robot aspiradora"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}:"

SRC_URI = "file://CMakeLists.txt \
           file://robot-server.c \
           file://robot-server.service"

S = "${WORKDIR}"

inherit cmake systemd

SYSTEMD_SERVICE:${PN} = "robot-server.service"
SYSTEMD_AUTO_ENABLE = "enable"

RDEPENDS:${PN} = "systemd"