#include <Arduino.h>
#include <cstdint>

namespace {

    enum class LedState : uint8_t { kOff, kOn };

    struct LedConfig {
        static constexpr uint8_t kPin = 16;
        static constexpr uint32_t kBlinkIntervalMs = 1000;
    };

    class Led {
    public:
        explicit Led(uint8_t pin) : pin_(pin) {}
        [[nodiscard]] LedState state() const { return state_; }
        void init();
        void set(LedState state);

    private:
        const uint8_t pin_;
        LedState state_ = LedState::kOff;
    };

    void Led::init() {
        pinMode(pin_, OUTPUT);
        set(LedState::kOff);
    }

    void Led::set(LedState state) {
        state_ = state;
        digitalWrite(pin_, (state_ == LedState::kOn) ? HIGH : LOW);
    }

    Led led(LedConfig::kPin);

}  // namespace

void setup() {
    led.init();
}

void loop() {
    static uint32_t lastToggleMs = millis();
    const uint32_t currentMs = millis();

    if (currentMs - lastToggleMs >= LedConfig::kBlinkIntervalMs) {
        lastToggleMs += LedConfig::kBlinkIntervalMs;
        led.set((led.state() == LedState::kOn) ? LedState::kOff : LedState::kOn);
    }
}