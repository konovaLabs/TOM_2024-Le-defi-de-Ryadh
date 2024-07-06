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
}

void register_hall_callback(void (*callback)(void))
{
    hall_callback = callback;
}