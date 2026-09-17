#ifndef GPIO_H
#define GPIO_H

void export_gpio(int pin);
void unexport_gpio(int pin);
int pinMode(int pin, int mode);
int digitalWrite(int pin, int value);
int digitalRead(int pin);
void blink(int pin, double freq, double duration);

#endif
