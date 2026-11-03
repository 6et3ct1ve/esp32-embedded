#include <Arduino.h>
#include <cstdint>

namespace {

constexpr uint8_t kRelayPin = 17;
constexpr uint8_t kLdrPin = 6;
constexpr uint32_t kToggleIntervalMs = 1000;
constexpr uint32_t kSerialBaud = 115200;

}  // namespace

void setup() {
    Serial.begin(kSerialBaud);
    pinMode(kRelayPin, OUTPUT);
    digitalWrite(kRelayPin, LOW);
}

void loop() {
    static uint32_t lastToggleMs = millis();
    static bool pinHigh = false;
    const uint32_t now = millis();
    if (now - lastToggleMs < kToggleIntervalMs) {
        return;
    }
    lastToggleMs += kToggleIntervalMs;

    pinHigh = !pinHigh;
    digitalWrite(kRelayPin, pinHigh ? HIGH : LOW);
    const uint16_t rawAdc = analogRead(kLdrPin);
    Serial.printf("GPIO %u: %s, LDR: %u\n", kRelayPin, pinHigh ? "HIGH" : "LOW", rawAdc);
}