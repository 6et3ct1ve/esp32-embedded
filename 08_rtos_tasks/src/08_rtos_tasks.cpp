#include <Arduino.h>
#include <cstdint>
#include <iterator>

namespace {

struct BlinkConfig {
    uint8_t pin = 0;
    uint32_t intervalMs = 0;
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

struct BlinkTaskParams {
    Led* led = nullptr;
    uint32_t intervalMs = 0;
};

Led leds[] = {
    Led(kBlinkConfigs[0].pin),
    Led(kBlinkConfigs[1].pin),
    Led(kBlinkConfigs[2].pin),
};
static_assert(std::size(leds) == std::size(kBlinkConfigs));

BlinkTaskParams blinkTaskParams[] = {
    {.led = &leds[0], .intervalMs = kBlinkConfigs[0].intervalMs},
    {.led = &leds[1], .intervalMs = kBlinkConfigs[1].intervalMs},
    {.led = &leds[2], .intervalMs = kBlinkConfigs[2].intervalMs},
};
static_assert(std::size(blinkTaskParams) == std::size(kBlinkConfigs));

}  // namespace

void setup() {
    for (auto& led : leds) {
        led.init();
    }
}

void loop() {
}
