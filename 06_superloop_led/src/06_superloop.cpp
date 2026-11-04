#include <Arduino.h>
#include <cstdint>

namespace {

    enum class LedState : uint8_t { kOff, kOn };

    struct LedConfig {
        static constexpr uint8_t kPin = 16;
        static constexpr uint32_t kBlinkIntervalMs = 1000;

    };

}  // namespace

void setup() {
    pinMode(LedConfig::kPin, OUTPUT);
}

void loop() {
    static uint32_t lastToggleMs = millis();
}