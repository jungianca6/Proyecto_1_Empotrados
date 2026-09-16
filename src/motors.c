#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "motors.h"

static void sysfs_write(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

static void gpio_export(int pin) {
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", pin);
    sysfs_write("/sys/class/gpio/export", buf);
}

static void gpio_direction(int pin, const char *dir) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    sysfs_write(path, dir);
}

static void gpio_set(int pin, int val) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    sysfs_write(path, val ? "1" : "0");
}

static void pwm_init_channel(int channel) {
    char buf[10], path[60];
    snprintf(buf, sizeof(buf), "%d", channel);
    sysfs_write("/sys/class/pwm/pwmchip0/export", buf);

    usleep(100000); // 100ms para asegurar creación del nodo

    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/period", channel);
    snprintf(buf, sizeof(buf), "%d", PWM_PERIOD_NS);
    sysfs_write(path, buf);

    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/enable", channel);
    sysfs_write(path, "1");
}

static void pwm_set_duty(int channel, int duty_ns) {
    char path[60], buf[20];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", channel);
    snprintf(buf, sizeof(buf), "%d", duty_ns);
    sysfs_write(path, buf);
}

int motors_init(void) {
    // 1. Exportar e inicializar pines de dirección
    int gpios[] = {GPIO_IN1, GPIO_IN2, GPIO_IN3, GPIO_IN4};
    for (int i = 0; i < 4; i++) {
        gpio_export(gpios[i]);
        gpio_direction(gpios[i], "out");
        gpio_set(gpios[i], 0);
    }

    // 2. Inicializar canales PWM 0 y 1
    pwm_init_channel(0);
    pwm_init_channel(1);
    set_motor_speeds(0, 0);

    return 0;
}

void set_motor_speeds(int speed_left_pct, int speed_right_pct) {
    if (speed_left_pct < 0) speed_left_pct = 0;
    if (speed_left_pct > 100) speed_left_pct = 100;
    if (speed_right_pct < 0) speed_right_pct = 0;
    if (speed_right_pct > 100) speed_right_pct = 100;

    int duty_left = (PWM_PERIOD_NS * speed_left_pct) / 100;
    int duty_right = (PWM_PERIOD_NS * speed_right_pct) / 100;

    pwm_set_duty(0, duty_left);  // PWM0: Izquierdo
    pwm_set_duty(1, duty_right); // PWM1: Derecho
}

void robot_move(RobotDirection dir, int speed_pct) {
    switch (dir) {
        case ROBOT_FORWARD:
            gpio_set(GPIO_IN1, 1); gpio_set(GPIO_IN2, 0);
            gpio_set(GPIO_IN3, 1); gpio_set(GPIO_IN4, 0);
            set_motor_speeds(speed_pct, speed_pct);
            break;
        case ROBOT_BACKWARD:
            gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 1);
            gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 1);
            set_motor_speeds(speed_pct, speed_pct);
            break;
        case ROBOT_TURN_LEFT: // Giro diferencial sobre su propio eje
            gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 1);
            gpio_set(GPIO_IN3, 1); gpio_set(GPIO_IN4, 0);
            set_motor_speeds(speed_pct, speed_pct);
            break;
        case ROBOT_TURN_RIGHT:
            gpio_set(GPIO_IN1, 1); gpio_set(GPIO_IN2, 0);
            gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 1);
            set_motor_speeds(speed_pct, speed_pct);
            break;
        case ROBOT_STOP:
        default:
            gpio_set(GPIO_IN1, 0); gpio_set(GPIO_IN2, 0);
            gpio_set(GPIO_IN3, 0); gpio_set(GPIO_IN4, 0);
            set_motor_speeds(0, 0);
            break;
    }
}

void motors_cleanup(void) {
    robot_move(ROBOT_STOP, 0);
    sysfs_write("/sys/class/pwm/pwmchip0/pwm0/enable", "0");
    sysfs_write("/sys/class/pwm/pwmchip0/pwm1/enable", "0");
}