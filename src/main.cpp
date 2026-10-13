#include <Arduino.h>

constexpr uint8_t LED_RED = 15;
constexpr uint8_t LED_BLUE = 16;
constexpr uint32_t SEC = 1000;

void setup() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void delayTime(bool led_red, bool led_blue, uint32_t durationMs) {
    if (led_red)
    {
        digitalWrite(LED_RED, HIGH); 
    } else {
        digitalWrite(LED_RED, LOW);
    }
    if (led_blue)
    {
        digitalWrite(LED_BLUE, HIGH); 
    } else {
        digitalWrite(LED_BLUE, LOW);
    }
    delay(durationMs);
}

void loop() {
    delayTime(true, false, SEC);
    delayTime(false, true, SEC);

    delayTime(false, false, SEC/2);
    delayTime(false, true, SEC*1.5);
    delayTime(false, false, SEC/2);
    delayTime(false, true, SEC*1.5);
    delayTime(false, false, SEC/2);

    delayTime(true, false, SEC/2);
    delayTime(false, false, SEC);
    delayTime(true, false, SEC/2);
    delayTime(false, false, SEC);
    delayTime(true, false, SEC/2);
    delayTime(false, false, SEC);

    delayTime(true, false, SEC/2);
    delayTime(false, true, SEC/2);
    delayTime(false, false, SEC/4);
    delayTime(false,true, SEC/2);
    delayTime(true, false, SEC/2);
    delayTime(false, false, SEC*2);
}