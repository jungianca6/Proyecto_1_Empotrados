#ifndef SENSORS_H
#define SENSORS_H

#define GPIO_BASE 512
#define GPIO_IR_FRONT 24 // Pin físico 18 -> Sysfs 536
#define GPIO_IR_SIDE 11 // Pin físico 23 -> Sysfs 523

int sensors_init(void);
int sensor_obstacle_detected(void); // Retorna 1 si hay obstáculo, 0 si está despejado
int sensor_side_obstacle_detected(void);


#endif // SENSORS_H