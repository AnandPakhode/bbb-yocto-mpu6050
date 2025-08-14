SUMMARY = "mpu6050 sensor"

LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


SRC_URI = "file://mpu6050.c"

DEPENDS = "libgpiod"


S = "${WORKDIR}"

do_compile() {
    ls -l ${S}  # Debugging step to check if mpu6050.c is there
    ${CC} ${CFLAGS} ${LDFLAGS} -o ${S}/dht11 ${S}/mpu6050.c -lgpiod
}


do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/dht11 ${D}${bindir}/mpu5050
}