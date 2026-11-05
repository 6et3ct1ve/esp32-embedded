#include <Arduino.h>
#include <cstdint>

namespace {

struct LedConfig {
    static constexpr uint8_t kPin = 16;
    static constexpr uint32_t kBlinkIntervalMs = 1000;
};

struct ButtonConfig {
    static constexpr uint8_t kPin = 17;
    static constexpr uint32_t kDebounceIntervalMs = 50;
};

struct ProfilingConfig {
    static constexpr uint32_t kSerialBaud = 115200;
    static constexpr uint32_t kReportEveryIterations = 1000;
};

enum class LedState : uint8_t { kOff, kOn };

class Led {
public:
    explicit Led(uint8_t pin) : pin_(pin) {}
    Led(const Led&) = delete;
    Led& operator=(const Led&) = delete;

    void init();
    void set(LedState newState);
    [[nodiscard]] LedState state() const { return state_; }

private:
    const uint8_t pin_;
    LedState state_ = LedState::kOff;
};

void Led::init() {
    pinMode(pin_, OUTPUT);
    set(LedState::kOff);
}

void Led::set(LedState newState) {
    state_ = newState;
    digitalWrite(pin_, (state_ == LedState::kOn) ? HIGH : LOW);
}

Led led(LedConfig::kPin);
volatile bool buttonPressed = false;

void IRAM_ATTR buttonIsr() {
    buttonPressed = true;
}

}  // namespace

void setup() {
    pinMode(ButtonConfig::kPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ButtonConfig::kPin), buttonIsr, FALLING);
    led.init();
    Serial.begin(ProfilingConfig::kSerialBaud);
}

void loop() {
    const uint32_t iterationStartUs = micros();

    static uint32_t lastToggleMs = millis();
    const uint32_t currentMs = millis();

    if (currentMs - lastToggleMs >= LedConfig::kBlinkIntervalMs) {
        lastToggleMs += LedConfig::kBlinkIntervalMs;
        led.set((led.state() == LedState::kOn) ? LedState::kOff : LedState::kOn);
    }

    static uint32_t totalLoopTimeUs = 0;
    static uint32_t iterationCount = 0;
    totalLoopTimeUs += (micros() - iterationStartUs);
    iterationCount++;

    if (iterationCount >= ProfilingConfig::kReportEveryIterations) {
        const float averageLoopTimeUs = static_cast<float>(totalLoopTimeUs) / iterationCount;
        Serial.printf("Average loop time: %.2f us\n", averageLoopTimeUs);
        totalLoopTimeUs = 0;
        iterationCount = 0;
    }
}