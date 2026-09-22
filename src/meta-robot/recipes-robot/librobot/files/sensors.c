#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sensors.h"

static int get_gpio_pin(int pin) {
    return (pin < 100) ? (GPIO_BASE + pin) : pin;
}

static void sysfs_write(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

int sensors_init(void) {
    int pin = get_gpio_pin(GPIO_IR_FRONT);
    char buf[16], path[60];

    // Exportar GPIO 24 (536)
    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/export", buf);

    // Configurar como ENTRADA
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    sysfs_write(path, "in");

    return 0;
}

int sensor_obstacle_detected(void) {
    int pin = get_gpio_pin(GPIO_IR_FRONT);
    char path[60], val = '1';
    
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        read(fd, &val, 1);
        close(fd);
    }

    // Este módulo FC-51 entrega '0' (LOW) al detectar obstáculo y '1' (HIGH) cuando está despejado
    return (val == '0') ? 1 : 0;
}