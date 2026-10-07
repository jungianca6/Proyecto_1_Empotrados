#ifndef BRUSHES_H
#define BRUSHES_H

#define GPIO_BASE 512

/*
 * Entradas del canal del puente H dedicado
 * a los motores de los cepillos.
 */
#define GPIO_BRUSH_IN1 21
#define GPIO_BRUSH_IN2 26

int brushes_init(void);
void brushes_on(void);
void brushes_off(void);
void brushes_cleanup(void);

#endif
