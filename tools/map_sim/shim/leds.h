#ifndef SHIM_LEDS_H
#define SHIM_LEDS_H

/* Sólo para el simulador y la verificación de sintaxis: en el robot se usa el leds.h real. */
int  leds_init(void);
void leds_cleanup(void);
void led_system_set(int on);
void led_autonomous_set(int on);
void led_manual_set(int on);
void led_obstacle_set(int on);

#endif
