#include <Arduino.h>
#include <cstdint>

namespace {

struct BlinkConfig {
    uint8_t pin;
    uint32_t intervalMs;
};

constexpr BlinkConfig kBlinkConfigs[] = {
    {.pin = 16, .intervalMs = 200},
    {.pin = 17, .intervalMs = 500},
    {.pin = 18, .intervalMs = 1000},
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

}  // namespace

void setup() {
}

void loop() {

}