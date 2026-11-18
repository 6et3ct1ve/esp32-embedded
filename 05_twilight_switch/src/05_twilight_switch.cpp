#include <Arduino.h>
#include <cstdint>

namespace {

constexpr uint8_t kRelayPin = 17;
constexpr uint8_t kLdrPin = 6;
constexpr uint32_t kSampleIntervalMs = 100;
constexpr uint32_t kSerialBaud = 115200;
constexpr uint16_t kDarkThreshold = 50;
constexpr uint16_t kLightThreshold = 2000;

enum class LightZone : uint8_t { kDark, kMedium, kLight };

LightZone classify(uint16_t raw) {
    if (raw < kDarkThreshold) {
        return LightZone::kDark;
    }
    if (raw > kLightThreshold) {
        return LightZone::kLight;
    }
    return LightZone::kMedium;
}

const char* toString(LightZone zone) {
    switch (zone) {
        case LightZone::kDark:
            return "DARK";
        case LightZone::kMedium:
            return "MEDIUM";
        case LightZone::kLight:
            return "LIGHT";
    }
    return "?";
}

}  // namespace

void setup() {
    Serial.begin(kSerialBaud);
    pinMode(kRelayPin, OUTPUT);
    digitalWrite(kRelayPin, LOW);
}

void loop() {
    static uint32_t lastSampleMs = millis();
    static bool relayOn = false;
    const uint32_t now = millis();
    if (now - lastSampleMs < kSampleIntervalMs) {
        return;
    }
    lastSampleMs += kSampleIntervalMs;

    const uint16_t rawAdc = analogRead(kLdrPin);
    const LightZone zone = classify(rawAdc);
    if (zone == LightZone::kDark) {
        relayOn = true;
    } else if (zone == LightZone::kLight) {
        relayOn = false;
    }

    digitalWrite(kRelayPin, relayOn ? HIGH : LOW);
    Serial.printf("LDR: %4u (%s), relay: %s\n", rawAdc, toString(zone), relayOn ? "ON" : "OFF");
}
