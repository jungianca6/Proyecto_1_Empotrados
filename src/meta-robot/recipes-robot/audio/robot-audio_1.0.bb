SUMMARY = "Biblioteca de audio del robot"
DESCRIPTION = "Reproduccion MP3 y notificaciones mediante ALSA"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://CMakeLists.txt \
    file://robot_audio.c \
    file://audio_test.c \
    file://audio_daemon.c \
    file://audioctl.c \
    file://robot_audio.h \
    file://robot-audio.service \
    file://notifications/system-start.mp3 \
    file://notifications/autonomous-start.mp3 \
    file://notifications/obstacle.mp3 \
    file://notifications/manual-mode.mp3 \
"

S = "${WORKDIR}"

DEPENDS = "alsa-lib mpg123"

inherit cmake pkgconfig systemd

EXTRA_OECMAKE += "-DBUILD_TESTING=OFF -DSYSTEMD_UNIT_DIR=${systemd_system_unitdir}"

RDEPENDS:${PN} = "alsa-lib"

SYSTEMD_SERVICE:${PN} = "robot-audio.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install:append() {
    install -d ${D}${datadir}/robot-audio
    install -m 0644 ${WORKDIR}/notifications/*.mp3 ${D}${datadir}/robot-audio/
}

FILES:${PN} += " \
    ${libdir}/librobot-audio.so.* \
    ${bindir}/audio-test \
    ${bindir}/audio-daemon \
    ${bindir}/audioctl \
    ${systemd_system_unitdir}/robot-audio.service \
    ${datadir}/robot-audio/*.mp3 \
"

FILES:${PN}-dev += " \
    ${libdir}/librobot-audio.so \
    ${includedir}/robot_audio.h \
"
