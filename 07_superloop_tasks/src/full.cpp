#include <Arduino.h>
#include <cstdint>
#include <iterator>

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

class Blinker {
public:
    explicit Blinker(BlinkConfig config)
        : led_(config.pin), intervalMs_(config.intervalMs) {}

    void init();
    void update(uint32_t nowMs);

private:
    Led led_;
    const uint32_t intervalMs_;
    uint32_t lastToggleMs_ = 0;
};

void Blinker::init() {
    led_.init();
    lastToggleMs_ = millis();
}

void Blinker::update(uint32_t nowMs) {
    if (nowMs - lastToggleMs_ >= intervalMs_) {
        lastToggleMs_ += intervalMs_;
        led_.set((led_.state() == LedState::kOn) ? LedState::kOff : LedState::kOn);
    }
}

Blinker blinkers[] = {
    Blinker{kBlinkConfigs[0]},
    Blinker{kBlinkConfigs[1]},
    Blinker{kBlinkConfigs[2]},
};
static_assert(std::size(blinkers) == std::size(kBlinkConfigs));

}  // namespace

void setup() {
}

void loop() {

}