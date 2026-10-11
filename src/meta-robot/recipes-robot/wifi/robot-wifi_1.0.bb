SUMMARY = "Configuracion automatica de Wi-Fi y llave SSH"
LICENSE = "CLOSED"

SRC_URI = " \
    file://robot-wifi.init \
    file://robot-wifi.service \
    file://25-wlan0.network \
    file://wpa_supplicant.conf \
    file://authorized_keys \
"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE:${PN} = "robot-wifi.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
    # Script de arranque Wi-Fi
    install -d ${D}${sbindir}
    install -m 0755 \
        ${WORKDIR}/robot-wifi.init \
        ${D}${sbindir}/robot-wifi-start

    # Configuracion wpa_supplicant
    install -d ${D}${sysconfdir}/wpa_supplicant
    install -m 0600 \
        ${WORKDIR}/wpa_supplicant.conf \
        ${D}${sysconfdir}/wpa_supplicant/wpa_supplicant.conf

    # Configuracion systemd-networkd
    install -d ${D}${sysconfdir}/systemd/network
    install -m 0644 \
        ${WORKDIR}/25-wlan0.network \
        ${D}${sysconfdir}/systemd/network/25-wlan0.network

    # Servicio systemd
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 \
        ${WORKDIR}/robot-wifi.service \
        ${D}${systemd_system_unitdir}/robot-wifi.service

    # Llave SSH
    install -d ${D}/root/.ssh
    install -m 0600 \
        ${WORKDIR}/authorized_keys \
        ${D}/root/.ssh/authorized_keys
}

FILES:${PN} += " \
    ${sbindir}/robot-wifi-start \
    ${sysconfdir}/wpa_supplicant/wpa_supplicant.conf \
    ${sysconfdir}/systemd/network/25-wlan0.network \
    ${systemd_system_unitdir}/robot-wifi.service \
    /root/.ssh/authorized_keys \
"
