#ifndef __BOARD_H
#define __BOARD_H

#include "Arduino.h"

#define HALL_SENSOR_PIN (1)
#define VIRTUAL_VCC_PIN (8)
#define GPIO_WHEEL_POSITION (7)

void board_init(void);
void register_hall_callback(void (*callback)(void));
void leds_set_color(uint8_t red, uint8_t green, uint8_t blue);
void leds_clear(void);

#endif /* __BOARD_H */