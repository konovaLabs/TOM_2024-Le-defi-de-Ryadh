#include "board.h"

static void (*hall_callback)(void) = NULL;

static void _isr_hall(void)
{
    if (hall_callback != NULL) {
        hall_callback();
    }
}

void board_init(void)
{
    pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), _isr_hall, FALLING);
    pinMode(VIRTUAL_VCC_PIN, OUTPUT);
    digitalWrite(VIRTUAL_VCC_PIN, HIGH);
    pinMode(GPIO_WHEEL_POSITION, INPUT_PULLDOWN);
}

void register_hall_callback(void (*callback)(void))
{
    hall_callback = callback;
}