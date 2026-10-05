#ifndef LEDS_H
#define LEDS_H

#define GPIO_BASE 512

#define GPIO_LED_SYSTEM      5
#define GPIO_LED_AUTONOMOUS  6
#define GPIO_LED_MANUAL      16
#define GPIO_LED_OBSTACLE    20

int leds_init(void);

void led_system_set(int on);
void led_autonomous_set(int on);
void led_manual_set(int on);
void led_obstacle_set(int on);

void leds_cleanup(void);

#endif // LEDS_H
