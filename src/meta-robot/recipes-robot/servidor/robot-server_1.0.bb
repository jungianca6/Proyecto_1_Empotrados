SUMMARY = "Servidor web del robot aspiradora"
DESCRIPTION = "Servidor web basado en uhttpd y CGI para el robot aspiradora"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://CMakeLists.txt \
    file://cgi/login.c \
    file://cgi/register.c \
    file://cgi/status.c \
    file://cgi/mode.c \
    file://cgi/motors.c \
    file://cgi/sensors.c \
    file://cgi/leds.c \
    file://cgi/audiolist.c \
    file://cgi/audioplay.c \
    file://cgi/audiopause.c \
    file://cgi/audiostop.c \
    file://cgi/audiovolume.c \
    file://cgi/map.c \
    file://www/index.html \
    file://www/css/style.css \
    file://www/js/app.js \
    file://robot-server.service \
    file://lib/session/session.c \
    file://lib/session/session.h \
    file://lib/stub/librobot_stub.c \
    file://lib/db/database.c \
    file://lib/db/database.h \
"

S = "${WORKDIR}"


DEPENDS = " \
    cjson \
    librobot \
    sqlite3 \
"

RDEPENDS:${PN} += " \
    uhttpd \
    sqlite3 \
"

inherit cmake pkgconfig systemd

# stub = librobot_stub (desarrollo); real = librobot de la imagen.
# Cambiar a "real" cuando librobot implemente la API robot_*.
PACKAGECONFIG ??= "real"
PACKAGECONFIG[stub] = "-DUSE_STUB=ON,-DUSE_STUB=OFF,,"
PACKAGECONFIG[real] = "-DUSE_STUB=OFF,-DUSE_STUB=ON,,librobot"

SYSTEMD_SERVICE:${PN} = "robot-server.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/robot-server.service \
        ${D}${systemd_system_unitdir}/robot-server.service
}

FILES:${PN} += " \
    /www \
    /www/cgi-bin \
    /www/cgi-bin/* \
    ${systemd_system_unitdir}/robot-server.service \
"