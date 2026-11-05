#include <Arduino.h>
#include <atomic>
#include <cstdint>

namespace {

struct LedConfig {
    static constexpr uint8_t kPin = 16;
    static constexpr uint32_t kBlinkIntervalMs = 1000;
};

struct ButtonConfig {
    static constexpr uint8_t kPin = 17;
    static constexpr uint32_t kDebounceIntervalMs = 10;
};

struct ProfilingConfig {
    static constexpr uint32_t kSerialBaud = 115200;
    static constexpr uint32_t kReportEveryIterations = 1000;
};

enum class LedState : uint8_t { kOff, kOn };

enum class LedMode : uint8_t { kBlink, kOn, kOff };

LedMode nextMode(LedMode current) {
    switch(current){
        case LedMode::kBlink:
            return LedMode::kOn;
        case LedMode::kOn:
            return LedMode::kOff;
        case LedMode::kOff:
            return LedMode::kBlink;
    }
    return LedMode::kBlink;
}

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
std::atomic<bool> buttonPressed{false};
static_assert(std::atomic<bool>::is_always_lock_free);

void IRAM_ATTR buttonIsr() {
    buttonPressed.store(true);
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
    static LedMode mode = LedMode::kBlink;
    static bool pendingButtonPress = false;
    static uint32_t lastEdgeMs = 0;
    const uint32_t currentMs = millis();
    
    if (buttonPressed.exchange(false)) {
        pendingButtonPress = true;
        lastEdgeMs = currentMs;
    }

    if (pendingButtonPress && (currentMs - lastEdgeMs >= ButtonConfig::kDebounceIntervalMs)) {
        pendingButtonPress = false;
        if (digitalRead(ButtonConfig::kPin) == LOW) {
            mode = nextMode(mode);
            if (mode == LedMode::kBlink) {
                lastToggleMs = currentMs;
            }
        }
    }

    switch (mode) {
        case LedMode::kBlink:
            if (currentMs - lastToggleMs >= LedConfig::kBlinkIntervalMs) {
                lastToggleMs += LedConfig::kBlinkIntervalMs;
                led.set((led.state() == LedState::kOn) ? LedState::kOff : LedState::kOn);
            }
            break;
        case LedMode::kOn:
            led.set(LedState::kOn);
            break;
        case LedMode::kOff:
            led.set(LedState::kOff);
            break;
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