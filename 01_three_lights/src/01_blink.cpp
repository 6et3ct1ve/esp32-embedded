#include <Arduino.h>

constexpr uint8_t kRedBlue = 15;
constexpr uint8_t kGreen = 17;
constexpr uint32_t kSpeedTwoCoeff = 2;

enum class RedBlueState : uint8_t {
    kOff, kRed, kBlue
};

struct LedStep {
    RedBlueState redBlue = RedBlueState::kOff;
    bool green = false;
    uint32_t durationMs = 0;
};

constexpr LedStep kLedSteps[] = {
    {.redBlue = RedBlueState::kRed, .durationMs = 1000},
    {.green = true, .durationMs = 1000},
    {.redBlue = RedBlueState::kBlue, .durationMs = 1000},
    {.durationMs = 500},
    {.redBlue = RedBlueState::kBlue, .durationMs = 1500},
    {.durationMs = 500},
    {.redBlue = RedBlueState::kBlue, .durationMs = 1500},
    {.green = true, .durationMs = 500},
    {.redBlue = RedBlueState::kRed, .durationMs = 500},
    {.durationMs = 1000},
    {.redBlue = RedBlueState::kRed, .durationMs = 500},
    {.durationMs = 1000},
    {.green = true, .durationMs = 500},
    {.durationMs = 1000},
    {.redBlue = RedBlueState::kRed, .durationMs = 500},
    {.redBlue = RedBlueState::kBlue, .durationMs = 500},
    {.durationMs = 250},
    {.redBlue = RedBlueState::kBlue, .durationMs = 500},
    {.redBlue = RedBlueState::kRed, .durationMs = 500},
    {.green = true, .durationMs = 500},
    {.durationMs = 2000},
};

void setup() {
    pinMode(kRedBlue, OUTPUT);
    pinMode(kGreen, OUTPUT);
}

void ledState(LedStep step, bool speedTwo) {
    switch (step.redBlue) {
        case RedBlueState::kOff:
            pinMode(kRedBlue, INPUT);
            break;
        case RedBlueState::kRed:
            pinMode(kRedBlue, OUTPUT);
            digitalWrite(kRedBlue, HIGH);
            break;
        case RedBlueState::kBlue:
            pinMode(kRedBlue, OUTPUT);
            digitalWrite(kRedBlue, LOW);
            break;
    }
    digitalWrite(kGreen, step.green ? HIGH : LOW);
    delay(speedTwo ? step.durationMs / kSpeedTwoCoeff : step.durationMs);
}

void loop() {
    static bool speedTwo = false;
    for (const auto& step : kLedSteps) {
        ledState(step, speedTwo);
    }
    speedTwo = !speedTwo;
}