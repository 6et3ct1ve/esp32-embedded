#include <Arduino.h>
#include <cstdint>

namespace {

    constexpr uint8_t kLdrPin = 6;
    constexpr float kAdcMax = 4095.0f;
    constexpr float kVrefMv = 3100.0f;
    constexpr uint32_t kSampleIntervalMs = 100;
    constexpr uint32_t kSerialBaud = 115200;

}  // namespace

void setup() {
    Serial.begin(kSerialBaud);
    Serial.printf("RAW   Calc, mV   Meas, mV   Error, %%\n");
}

void loop() {
    static uint32_t lastSampleMs = millis();
    const uint32_t now = millis();
    if (now - lastSampleMs < kSampleIntervalMs) {
        return;
    }
    lastSampleMs += kSampleIntervalMs;

    const uint16_t rawAdc = analogRead(kLdrPin);
    const float calcMv = (rawAdc / kAdcMax) * kVrefMv;
    const uint32_t measMv = analogReadMilliVolts(kLdrPin);

    if (measMv == 0) {
        Serial.printf("%4u   %8.2f   %8lu   %8s\n", rawAdc, calcMv, measMv, "-");
        return;
    }

    const float errorPercent = (static_cast<float>(measMv) - calcMv) / static_cast<float>(measMv) * 100.0f;
    Serial.printf("%4u   %8.2f   %8lu   %8.2f\n", rawAdc, calcMv, measMv, errorPercent);
}