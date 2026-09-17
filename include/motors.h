#ifndef MOTORS_H
#define MOTORS_H

#define GPIO_BASE 512

// Pines BCM para habilitación y dirección de motores
#define GPIO_ENA 12  // Pin físico 32 -> Sysfs 524
#define GPIO_ENB 13  // Pin físico 33 -> Sysfs 525
#define GPIO_IN1 17  // Pin físico 11 -> Sysfs 529
#define GPIO_IN2 27  // Pin físico 13 -> Sysfs 539
#define GPIO_IN3 22  // Pin físico 15 -> Sysfs 534
#define GPIO_IN4 23  // Pin físico 16 -> Sysfs 535

typedef enum {
    ROBOT_STOP = 0,
    ROBOT_FORWARD,
    ROBOT_BACKWARD,
    ROBOT_TURN_LEFT,
    ROBOT_TURN_RIGHT
} RobotDirection;

int motors_init(void);
void robot_move(RobotDirection dir, int speed_pct); // speed_pct > 0 activa los motores al 100%
void motors_cleanup(void);

#endif // MOTORS_H