#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "gpio.h"

// Offset base para el chip GPIO en Raspberry Pi 4 (Kernel Linux 5.15+)
#define GPIO_BASE 512

void export_gpio(int pin) {
    int real_pin = pin + GPIO_BASE;
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) return;
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d", real_pin);
    (void)write(fd, buffer, len);
    close(fd);
    usleep(100000); // Retardo para sincronizar sysfs
}

void unexport_gpio(int pin) {
    int real_pin = pin + GPIO_BASE;
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) return;
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d", real_pin);
    (void)write(fd, buffer, len);
    close(fd);
}

int pinMode(int pin, int mode) {
    int real_pin = pin + GPIO_BASE;
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", real_pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    if (mode == 1) {
        (void)write(fd, "out", 3);
    } else {
        (void)write(fd, "in", 2);
    }
    close(fd);
    return 0;
}

int digitalWrite(int pin, int value) {
    int real_pin = pin + GPIO_BASE;
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", real_pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    if (value)
        (void)write(fd, "1", 1);
    else
        (void)write(fd, "0", 1);
    close(fd);
    return 0;
}

int digitalRead(int pin) {
    int real_pin = pin + GPIO_BASE;
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", real_pin);
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    char val_str[3] = {0};
    (void)read(fd, val_str, 1);
    close(fd);
    return atoi(val_str);
}

void blink(int pin, double freq, double duration) {
    if (freq <= 0.0 || duration <= 0.0) return;
    int half_period_us = (int)(1000000.0 / (2.0 * freq));
    int total_cycles = (int)(freq * duration);
    pinMode(pin, 1);
    for (int i = 0; i < total_cycles; i++) {
        digitalWrite(pin, 1);
        usleep(half_period_us);
        digitalWrite(pin, 0);
        usleep(half_period_us);
    }
}
