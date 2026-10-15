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

static void sensor_gpio_init(int gpio) {
    int pin = get_gpio_pin(gpio);
    char buf[16], path[60];

    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/export", buf);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    sysfs_write(path, "in");
}

static int sensor_gpio_read(int gpio) {
    int pin = get_gpio_pin(gpio);
    char path[60], val = '1';

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);

    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        read(fd, &val, 1);
        close(fd);
    }

    // FC-51:
    // 0 = obstáculo
    // 1 = despejado
    return (val == '0') ? 1 : 0;
}

int sensors_init(void) {
    sensor_gpio_init(GPIO_IR_FRONT);
    sensor_gpio_init(GPIO_IR_SIDE);

    return 0;
}

int sensor_obstacle_detected(void) {
    return sensor_gpio_read(GPIO_IR_FRONT);
}

int sensor_side_obstacle_detected(void) {
    return sensor_gpio_read(GPIO_IR_SIDE);
}