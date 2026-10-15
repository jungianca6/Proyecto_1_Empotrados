#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "brushes.h"

static int get_gpio_pin(int pin)
{
    return (pin < 100) ? (GPIO_BASE + pin) : pin;
}

static void sysfs_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);

    if (fd < 0)
        return;

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

static void gpio_direction(int raw_pin, const char *dir)
{
    int pin = get_gpio_pin(raw_pin);
    char path[60];

    snprintf(path,
             sizeof(path),
             "/sys/class/gpio/gpio%d/direction",
             pin);

    sysfs_write(path, dir);
}

/*
 * Las señales pasan por optoacopladores 4N25,
 * por lo que la lógica queda invertida.
 *
 * Es la misma lógica utilizada en motors.c.
 */
static void gpio_set_inverted(int raw_pin, int val)
{
    int pin = get_gpio_pin(raw_pin);
    char path[60];

    snprintf(path,
             sizeof(path),
             "/sys/class/gpio/gpio%d/value",
             pin);

    sysfs_write(path, val ? "0" : "1");
}

int brushes_init(void)
{
    int gpios[] = {
        GPIO_BRUSH_IN1,
        GPIO_BRUSH_IN2
    };

    for (int i = 0; i < 2; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
    }

    brushes_off();

    return 0;
}

void brushes_on(void)
{
    /*
     * Los cepillos solo necesitan una dirección.
     *
     * Se utiliza exactamente la combinación que
     * ya sabemos que funciona en motors.c para
     * hacer girar un motor hacia adelante.
     */
    gpio_set_inverted(GPIO_BRUSH_IN1, 0);
    gpio_set_inverted(GPIO_BRUSH_IN2, 1);
}

void brushes_off(void)
{
    /*
     * Ambas entradas en estado de parada.
     */
    gpio_set_inverted(GPIO_BRUSH_IN1, 0);
    gpio_set_inverted(GPIO_BRUSH_IN2, 0);
}

void brushes_cleanup(void)
{
    int gpios[] = {
        GPIO_BRUSH_IN1,
        GPIO_BRUSH_IN2
    };

    brushes_off();

    for (int i = 0; i < 2; i++) {
        gpio_unexport(gpios[i]);
    }
}
