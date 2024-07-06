#include <Arduino.h>
#include "board.h"
#include "ble.h"

void setup()
{
    board_init();
    ble_init();
}

void loop()
{
    // put your main code here, to run repeatedly:
    delay(2000);
}