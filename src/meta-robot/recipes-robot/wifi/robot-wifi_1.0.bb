SUMMARY = "Configuracion e inicio automatico de Wi-Fi y llave SSH"
LICENSE = "CLOSED"

SRC_URI = " \
    file://robot-wifi.init \
    file://wpa_supplicant.conf \
    file://authorized_keys \
"

S = "${WORKDIR}"

do_install() {
    # 1. Instalar el script en /etc/init.d/
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/robot-wifi.init ${D}${sysconfdir}/init.d/wifi_start.sh

    # 2. Crear enlaces simbolicos de arranque directamente en la imagen
    install -d ${D}${sysconfdir}/rcS.d
    install -d ${D}${sysconfdir}/rc3.d
    install -d ${D}${sysconfdir}/rc5.d
    ln -sf ../init.d/wifi_start.sh ${D}${sysconfdir}/rcS.d/S99wifi_start.sh
    ln -sf ../init.d/wifi_start.sh ${D}${sysconfdir}/rc3.d/S99wifi_start.sh
    ln -sf ../init.d/wifi_start.sh ${D}${sysconfdir}/rc5.d/S99wifi_start.sh

    # 3. Instalar configuracion de Wi-Fi
    install -d ${D}${sysconfdir}/wpa_supplicant
    install -m 0600 ${WORKDIR}/wpa_supplicant.conf ${D}${sysconfdir}/wpa_supplicant/wpa_supplicant.conf

    # 4. Instalar llave SSH
    install -d ${D}/root/.ssh
    install -m 0600 ${WORKDIR}/authorized_keys ${D}/root/.ssh/authorized_keys
}

FILES:${PN} += " \
    ${sysconfdir}/init.d/wifi_start.sh \
    ${sysconfdir}/rcS.d/S99wifi_start.sh \
    ${sysconfdir}/rc3.d/S99wifi_start.sh \
    ${sysconfdir}/rc5.d/S99wifi_start.sh \
    ${sysconfdir}/wpa_supplicant/wpa_supplicant.conf \
    /root/.ssh/authorized_keys \
"