#include <Arduino.h>
#include <iterator>

constexpr uint8_t kWhite = 16;

void setup() {
    pinMode(kWhite, OUTPUT);
}

void loop() {
    static uint32_t stepStart = millis();
}