#include <Arduino.h>
#include "board.h"
#include "ble.h"

unsigned long lastUpdate = 0;
unsigned long last_wheel_update = 0;
unsigned long previouswheelUpdate = 0;
volatile bool data_update = false;
uint16_t wheel_rev_period;
const int debounce_period_ms = 10;

static csc_measurement_t csc_measurement;

static void _hall_clbk(void)
{
    static volatile int last_interrupt_ms = 0;

    if (millis() - last_interrupt_ms > debounce_period_ms)
    {
        data_update = true;
        last_interrupt_ms = millis();

        wheel_rev_period = millis() - last_wheel_update;
        last_wheel_update = millis();
        csc_measurement.cum_wheel_rev++;
        csc_measurement.last_wheel_event += wheel_rev_period;
    }
}

static void _on_ble_connection_changed(bool connected)
{
    if (connected)
    {
        Serial.println("BLE client connected.");
        ble_update_send_csc_measurement(&csc_measurement);
    }
    else
    {
        Serial.println("BLE client disconnected.");
    }
}

static void _measurements_update()
{
    if (data_update)
    {
        data_update = false;
#if 0
        Serial.print("Wheel_Rev_Period : ");
        Serial.println(wheel_rev_period);
        Serial.print("LastwheelUpdte : ");
        Serial.println(last_wheel_update);
        Serial.print("Cum_Wheel_Rev : ");
        Serial.println(csc_measurement.cum_wheel_rev);
#endif
        ble_update_send_csc_measurement(&csc_measurement);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ryadh Project - BLE Wheel Sensor");
    board_init();
    ble_init();
    register_hall_callback(_hall_clbk);
    ble_register_connection_callback(_on_ble_connection_changed);
}

void loop()
{
    _measurements_update();
    //_hall_clbk();
    delay(1);
}
