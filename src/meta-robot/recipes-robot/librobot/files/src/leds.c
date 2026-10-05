#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "leds.h"

static int get_gpio_pin(int pin)
{
    return (pin < 100) ? (GPIO_BASE + pin) : pin;
}

static void sysfs_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        return;
    }

    write(fd, value, strlen(value));
    close(fd);
}

static void gpio_export(int raw_pin)
{
    int pin = get_gpio_pin(raw_pin);
    char buf[16];

    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/export", buf);

    usleep(5000);
}

static void gpio_unexport(int raw_pin)
{
    int pin = get_gpio_pin(raw_pin);
    char buf[16];

    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/unexport", buf);
}

static void gpio_direction(int raw_pin, const char *direction)
{
    int pin = get_gpio_pin(raw_pin);
    char path[64];

    snprintf(
        path,
        sizeof(path),
        "/sys/class/gpio/gpio%d/direction",
        pin
    );

    sysfs_write(path, direction);
}

static void gpio_set(int raw_pin, int value)
{
    int pin = get_gpio_pin(raw_pin);
    char path[64];

    snprintf(
        path,
        sizeof(path),
        "/sys/class/gpio/gpio%d/value",
        pin
    );

    sysfs_write(path, value ? "1" : "0");
}

int leds_init(void)
{
    int gpios[] = {
        GPIO_LED_SYSTEM,
        GPIO_LED_AUTONOMOUS,
        GPIO_LED_MANUAL,
        GPIO_LED_OBSTACLE
    };

    for (int i = 0; i < 4; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
        gpio_set(gpios[i], 0);
    }

    return 0;
}

void led_system_set(int on)
{
    gpio_set(GPIO_LED_SYSTEM, on);
}

void led_autonomous_set(int on)
{
    gpio_set(GPIO_LED_AUTONOMOUS, on);
}

void led_manual_set(int on)
{
    gpio_set(GPIO_LED_MANUAL, on);
}

void led_obstacle_set(int on)
{
    gpio_set(GPIO_LED_OBSTACLE, on);
}

void leds_cleanup(void)
{
    int gpios[] = {
        GPIO_LED_SYSTEM,
        GPIO_LED_AUTONOMOUS,
        GPIO_LED_MANUAL,
        GPIO_LED_OBSTACLE
    };

    for (int i = 0; i < 4; i++) {
        gpio_set(gpios[i], 0);
        gpio_unexport(gpios[i]);
    }
}