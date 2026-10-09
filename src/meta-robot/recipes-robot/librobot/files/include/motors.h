#ifndef MOTORS_H
#define MOTORS_H

#define GPIO_BASE 512

#define GPIO_ENA 12
#define GPIO_ENB 13
#define GPIO_IN1 17
#define GPIO_IN2 27
#define GPIO_IN3 22
#define GPIO_IN4 23

typedef enum {
    ROBOT_STOP = 0,
    ROBOT_FORWARD,
    ROBOT_BACKWARD,
    ROBOT_TURN_LEFT,
    ROBOT_TURN_RIGHT
} RobotDirection;

int motors_init(void);

/*
 * En esta versión no se usa PWM.
 * speed_pct <= 0 apaga los motores.
 * speed_pct > 0 activa los motores a potencia completa.
 */
void robot_move(RobotDirection dir, int speed_pct);

void motors_cleanup(void);

#endif
