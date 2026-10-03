SUMMARY = "Biblioteca compartida librobot para motores, sensores, GPIO y FSM"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://CMakeLists.txt \
    file://include/librobot.h \
    file://include/motors.h \
    file://include/sensors.h \
    file://include/fsm.h \
    file://include/gpio.h \
    file://src/librobot.c \
    file://src/motors.c \
    file://src/sensors.c \
    file://src/fsm.c \
    file://src/gpio.c \
"

S = "${WORKDIR}"

inherit cmake

# Indicar a Yocto que los archivos .so sin version pertenecen al paquete runtime principal
SOLIBS = ".so"
SOLIBSDEV = ""

FILES:${PN} = "${libdir}/librobot.so"
FILES:${PN}-dev = "${includedir}"