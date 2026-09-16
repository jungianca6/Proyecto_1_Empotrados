#ifndef MOTORS_H
#define MOTORS_H

// Offset Base Kernel Linux = 512
// Asignación de Pines GPIO (Dirección):
// Motor Izquierdo: IN1 (GPIO 18 -> 530), IN2 (GPIO 23 -> 535)
// Motor Derecho:   IN3 (GPIO 24 -> 536), IN4 (GPIO 25 -> 537)
#define GPIO_IN1 530
#define GPIO_IN2 535
#define GPIO_IN3 536
#define GPIO_IN4 537

// Canales Hardware PWM (sysfs)
// PWM0 -> GPIO 12 (Motor Izquierdo Enable)
// PWM1 -> GPIO 13 (Motor Derecho Enable)
#define PWM_PERIOD_NS 1000000 // 1 kHz (1,000,000 ns)

typedef enum {
    ROBOT_STOP = 0,
    ROBOT_FORWARD,
    ROBOT_BACKWARD,
    ROBOT_TURN_LEFT,
    ROBOT_TURN_RIGHT
} RobotDirection;

int motors_init(void);
void set_motor_speeds(int speed_left_pct, int speed_right_pct);
void robot_move(RobotDirection dir, int speed_pct);
void motors_cleanup(void);

#endif