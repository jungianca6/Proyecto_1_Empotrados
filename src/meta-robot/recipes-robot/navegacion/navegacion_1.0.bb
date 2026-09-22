SUMMARY = "Librería de la aspiradora"
DESCRIPTION = "Librería dinámica para el robot aspiradora"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://CMakeLists.txt \
    file://fsm.c \
    file://fsm.h \
    file://main.c \
"

S = "${WORKDIR}"

inherit cmake