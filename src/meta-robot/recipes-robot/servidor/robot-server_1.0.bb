SUMMARY = "Servidor web del robot aspiradora"
DESCRIPTION = "Servidor web basado en uhttpd y CGI para el robot aspiradora"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://CMakeLists.txt \
    file://cgi/login.c \
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
"

S = "${WORKDIR}"

inherit cmake 

#systemd 

#SYSTEMD_SERVICE:${PN} = "robot-server.service"
#SYSTEMD_AUTO_ENABLE = "enable"

RDEPENDS:${PN} += "uhttpd"

FILES:${PN} += " \
    /www \
    /www/cgi-bin \
    /www/cgi-bin/* \
"