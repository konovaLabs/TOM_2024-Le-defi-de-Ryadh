#include "board.h"
#include <Adafruit_NeoPixel.h>

#define PIN_WS2812B 10 // ESP32 pin that connects to WS2812B
#define NUM_PIXELS 30  // The number of LEDs (pixels) on WS2812B

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

static void (*hall_callback)(void) = NULL;

static void _isr_hall(void)
{
    if (hall_callback != NULL)
    {
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

    ws2812b.begin(); // initialize WS2812B strip object (REQUIRED)
    ws2812b.clear(); // set all pixel colors to 'off'. It only takes effect if pixels.show() is called
    ws2812b.show();
}

void register_hall_callback(void (*callback)(void))
{
    hall_callback = callback;
}

void leds_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int pixel = 0; pixel < NUM_PIXELS; pixel++)
    {
        ws2812b.setPixelColor(pixel, ws2812b.Color(red, green, blue));
    }
    ws2812b.show();
}

void leds_clear(void)
{
    ws2812b.clear();
    ws2812b.show();
}