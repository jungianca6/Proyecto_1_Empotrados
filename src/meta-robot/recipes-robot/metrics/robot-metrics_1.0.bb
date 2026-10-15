SUMMARY = "Recolector de metricas del robot"
DESCRIPTION = "Mide automaticamente rootfs, boot, RAM y CPU durante el arranque"
LICENSE = "CLOSED"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://robot-metrics.sh \
    file://robot-metrics.service \
"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE:${PN} = "robot-metrics.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

RDEPENDS:${PN} += " \
    busybox \
"

do_install() {
    install -d ${D}${sbindir}

    install -m 0755 \
        ${WORKDIR}/robot-metrics.sh \
        ${D}${sbindir}/robot-metrics-collect

    install -d ${D}${systemd_system_unitdir}

    install -m 0644 \
        ${WORKDIR}/robot-metrics.service \
        ${D}${systemd_system_unitdir}/robot-metrics.service
}

FILES:${PN} += " \
    ${sbindir}/robot-metrics-collect \
    ${systemd_system_unitdir}/robot-metrics.service \
"