#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "vacuum.h"

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
 * Las señales pasan por optoacopladores,
 * por eso la lógica queda invertida.
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

int vacuum_init(void)
{
    int gpios[] = {
        GPIO_VACUUM_IN3,
        GPIO_VACUUM_IN4
    };

    for (int i = 0; i < 2; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
    }

    vacuum_off();

    return 0;
}

void vacuum_on(void)
{
    /*
     * Dirección fija:
     * el motor de aspiración gira siempre
     * en el mismo sentido.
     */
    gpio_set_inverted(GPIO_VACUUM_IN3, 0);
    gpio_set_inverted(GPIO_VACUUM_IN4, 1);
}

void vacuum_off(void)
{
    /*
     * Ambas entradas en estado de parada.
     */
    gpio_set_inverted(GPIO_VACUUM_IN3, 0);
    gpio_set_inverted(GPIO_VACUUM_IN4, 0);
}

void vacuum_cleanup(void)
{
    int gpios[] = {
        GPIO_VACUUM_IN3,
        GPIO_VACUUM_IN4
    };

    vacuum_off();

    for (int i = 0; i < 2; i++) {
        gpio_unexport(gpios[i]);
    }
}
