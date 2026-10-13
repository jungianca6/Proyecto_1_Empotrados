SUMMARY = "Configuracion de audio analogico para Raspberry Pi"
LICENSE = "CLOSED"

SRC_URI = " \
    file://snd-bcm2835.conf \
    file://snd_bcm2835.conf \
"

S = "${WORKDIR}"

do_install() {
    install -d ${D}${sysconfdir}/modprobe.d
    install -m 0644 ${WORKDIR}/snd-bcm2835.conf \
        ${D}${sysconfdir}/modprobe.d/snd-bcm2835.conf

    install -d ${D}${sysconfdir}/modules-load.d
    install -m 0644 ${WORKDIR}/snd_bcm2835.conf \
        ${D}${sysconfdir}/modules-load.d/snd_bcm2835.conf
}

FILES:${PN} += " \
    ${sysconfdir}/modprobe.d/snd-bcm2835.conf \
    ${sysconfdir}/modules-load.d/snd_bcm2835.conf \
"
