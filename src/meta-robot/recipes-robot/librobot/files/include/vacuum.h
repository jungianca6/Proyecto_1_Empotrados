#ifndef VACUUM_H
#define VACUUM_H

#define GPIO_BASE 512

/*
 * Segundo canal del puente H:
 * IN3 e IN4 controlan el motor de aspiración.
 */
#define GPIO_VACUUM_IN3 18
#define GPIO_VACUUM_IN4 19

int vacuum_init(void);
void vacuum_on(void);
void vacuum_off(void);
void vacuum_cleanup(void);

#endif