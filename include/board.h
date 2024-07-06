#ifndef __BOARD_H
#define __BOARD_H

#include "Arduino.h"

#define HALL_SENSOR_PIN        (1)


void board_init(void);
void register_hall_callback(void (*callback)(void));

#endif /* __BOARD_H */