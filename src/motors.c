#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "motors.h"

static int get_gpio_pin(int pin) {
    return (pin < 100) ? (GPIO_BASE + pin) : pin;
}

static void sysfs_write(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

static void gpio_export(int raw_pin) {
    int pin = get_gpio_pin(raw_pin);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/export", buf);
}

static void gpio_direction(int raw_pin, const char *dir) {
    int pin = get_gpio_pin(raw_pin);
    char path[60];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    sysfs_write(path, dir);
}

static void gpio_set(int raw_pin, int val) {
    int pin = get_gpio_pin(raw_pin);
    char path[60];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    sysfs_write(path, val ? "1" : "0");
}

int motors_init(void) {
    // Lista de todos los GPIOs a controlar (ENA, ENB e IN1..IN4)
    int gpios[] = {GPIO_ENA, GPIO_ENB, GPIO_IN1, GPIO_IN2, GPIO_IN3, GPIO_IN4};
    
    for (int i = 0; i < 6; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
        gpio_set(gpios[i], 0);
    }

    return 0;
}

void robot_move(RobotDirection dir, int speed_pct) {
    // Si la velocidad es 0 o la dirección es STOP, desactivar todo
    if (dir == ROBOT_STOP || speed_pct <= 0) {
        gpio_set(GPIO_ENA, 0);
        gpio_set(GPIO_ENB, 0);
        gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 0);
        gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 0);
        return;
    }

    // Activar etapas de potencia ENA y ENB
    gpio_set(GPIO_ENA, 1);
    gpio_set(GPIO_ENB, 1);

    switch (dir) {
        case ROBOT_FORWARD:
            gpio_set(GPIO_IN1, 1); gpio_set(GPIO_IN2, 0);
            gpio_set(GPIO_IN3, 1); gpio_set(GPIO_IN4, 0);
            break;
        case ROBOT_BACKWARD:
            gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 1);
            gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 1);
            break;
        case ROBOT_TURN_LEFT:
            gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 1);
            gpio_set(GPIO_IN3, 1); gpio_set(GPIO_IN4, 0);
            break;
        case ROBOT_TURN_RIGHT:
            gpio_set(GPIO_IN1, 1); gpio_set(GPIO_IN2, 0);
            gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 1);
            break;
        default:
            gpio_set(GPIO_ENA, 0);
            gpio_set(GPIO_ENB, 0);
            break;
    }
}

void motors_cleanup(void) {
    robot_move(ROBOT_STOP, 0);
}