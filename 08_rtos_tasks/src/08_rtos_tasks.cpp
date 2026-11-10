#include <Arduino.h>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iterator>

namespace {
    
constexpr uint32_t kStackSizeBytes = 2048;
constexpr UBaseType_t kLedBlinkPriority = 1;

struct BlinkConfig {
    uint8_t pin = 0;
    uint32_t intervalMs = 0;
    const char* name = nullptr;
};

constexpr BlinkConfig kBlinkConfigs[] = {
    {.pin = 16, .intervalMs = 200, .name = "blinkRed"},
    {.pin = 17, .intervalMs = 500, .name = "blinkYellow"},
    {.pin = 18, .intervalMs = 1000, .name = "blinkWhite"},
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
    const char* name = nullptr;
};

Led leds[] = {
    Led(kBlinkConfigs[0].pin),
    Led(kBlinkConfigs[1].pin),
    Led(kBlinkConfigs[2].pin),
};
static_assert(std::size(leds) == std::size(kBlinkConfigs));

BlinkTaskParams blinkTaskParams[] = {
    {.led = &leds[0], .intervalMs = kBlinkConfigs[0].intervalMs, .name = kBlinkConfigs[0].name},
    {.led = &leds[1], .intervalMs = kBlinkConfigs[1].intervalMs, .name = kBlinkConfigs[1].name},
    {.led = &leds[2], .intervalMs = kBlinkConfigs[2].intervalMs, .name = kBlinkConfigs[2].name},
};
static_assert(std::size(blinkTaskParams) == std::size(kBlinkConfigs));

void blinkTask(void* params) {
    auto* taskParams = static_cast<BlinkTaskParams*>(params);
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(taskParams->intervalMs);
    while (true) {
        taskParams->led->set(LedState::kOn);
        vTaskDelayUntil(&lastWake, period);
        taskParams->led->set(LedState::kOff);
        vTaskDelayUntil(&lastWake, period);
    }
}

}  // namespace

void setup() {
    for (auto& led : leds) {
        led.init();
    }
}

void loop() {
}
