# usrmerge: reubicar /bin, /sbin y /lib bajo /usr
do_install:append() {
	install -d ${D}/usr/bin ${D}/usr/sbin ${D}/usr/lib
	mv ${D}/bin/ipcalc.sh ${D}/usr/bin/
	mv ${D}/sbin/sysupgrade ${D}/sbin/firstboot ${D}/usr/sbin/
	cp -a ${D}/lib/. ${D}/usr/lib/
	rm -rf ${D}/lib ${D}/bin ${D}/sbin
}

FILES:${PN}-openwrt = "\
	/usr/lib/functions.sh \
	/usr/lib/functions/uci-defaults.sh \
	/usr/lib/functions/system.sh \
	/usr/bin/ipcalc.sh \
	${sysconfdir}/config \
"

FILES:${PN}-sysupgrade = "\
	${sysconfdir}/sysupgrade.conf \
	/usr/sbin/sysupgrade \
	/usr/lib/upgrade/* \
	/usr/sbin/firstboot \
"

# corrige el typo ${sysconfdir] del original
CONFFILES:${PN}-sysupgrade = "${sysconfdir}/sysupgrade.conf"
