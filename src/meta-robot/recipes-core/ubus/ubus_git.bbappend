# usrmerge: /sbin es un symlink a /usr/sbin, los symlinks de OpenWrt en /sbin sobran
do_install:append() {
	rm -rf ${D}/sbin
}
