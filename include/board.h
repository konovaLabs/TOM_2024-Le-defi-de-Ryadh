#ifndef __BOARD_H
#define __BOARD_H

#include "Arduino.h"

#define HALL_SENSOR_PIN (1)
#define VIRTUAL_VCC_PIN (8)
#define GPIO_WHEEL_POSITION (7)

void board_init(void);
void register_hall_callback(void (*callback)(void));

#endif /* __BOARD_H */