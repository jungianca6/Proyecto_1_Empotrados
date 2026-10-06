#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "motors.h"

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
 * IN1...IN4 pasan por los optoacopladores 4N25.
 * Por eso se utiliza lógica invertida.
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

/*
 * ENA y ENB se controlan directamente.
 */
static void gpio_set_direct(int raw_pin, int val)
{
    int pin = get_gpio_pin(raw_pin);
    char path[60];

    snprintf(path,
             sizeof(path),
             "/sys/class/gpio/gpio%d/value",
             pin);

    sysfs_write(path, val ? "1" : "0");
}

int motors_init(void)
{
    int gpios[] = {
        GPIO_ENA,
        GPIO_ENB,
        GPIO_IN1,
        GPIO_IN2,
        GPIO_IN3,
        GPIO_IN4
    };

    for (int i = 0; i < 6; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
    }

    robot_move(ROBOT_STOP, 0);

    return 0;
}

void robot_move(RobotDirection dir, int speed_pct)
{
    /*
     * Sin PWM:
     * cualquier velocidad positiva activa el canal
     * a potencia completa.
     */
    if (dir == ROBOT_STOP || speed_pct <= 0) {

        gpio_set_direct(GPIO_ENA, 0);
        gpio_set_direct(GPIO_ENB, 0);

        gpio_set_inverted(GPIO_IN1, 0);
        gpio_set_inverted(GPIO_IN2, 0);

        gpio_set_inverted(GPIO_IN3, 0);
        gpio_set_inverted(GPIO_IN4, 0);

        return;
    }

    gpio_set_direct(GPIO_ENA, 1);
    gpio_set_direct(GPIO_ENB, 1);

    switch (dir) {

        case ROBOT_FORWARD:

            gpio_set_inverted(GPIO_IN1, 0);
            gpio_set_inverted(GPIO_IN2, 1);

            gpio_set_inverted(GPIO_IN3, 1);
            gpio_set_inverted(GPIO_IN4, 0);

            break;

        case ROBOT_BACKWARD:

            gpio_set_inverted(GPIO_IN1, 1);
            gpio_set_inverted(GPIO_IN2, 0);

            gpio_set_inverted(GPIO_IN3, 0);
            gpio_set_inverted(GPIO_IN4, 1);

            break;

        case ROBOT_TURN_RIGHT:

            gpio_set_inverted(GPIO_IN1, 0);
            gpio_set_inverted(GPIO_IN2, 1);

            gpio_set_inverted(GPIO_IN3, 0);
            gpio_set_inverted(GPIO_IN4, 0);

            break;

        case ROBOT_TURN_LEFT:

            gpio_set_inverted(GPIO_IN1, 0);
            gpio_set_inverted(GPIO_IN2, 0);

            gpio_set_inverted(GPIO_IN3, 1);
            gpio_set_inverted(GPIO_IN4, 0);

            break;

        default:

            gpio_set_direct(GPIO_ENA, 0);
            gpio_set_direct(GPIO_ENB, 0);

            break;
    }
}

void motors_cleanup(void)
{
    int gpios[] = {
        GPIO_ENA,
        GPIO_ENB,
        GPIO_IN1,
        GPIO_IN2,
        GPIO_IN3,
        GPIO_IN4
    };

    robot_move(ROBOT_STOP, 0);

    for (int i = 0; i < 6; i++) {
        gpio_unexport(gpios[i]);
    }
}
