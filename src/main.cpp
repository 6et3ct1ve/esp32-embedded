#include <Arduino.h>

constexpr uint8_t kLedRed = 15;
constexpr uint8_t kLedBlue = 16;
constexpr uint8_t kLedGreen = 17;
constexpr uint32_t kSpeedTwoCoeff = 2;

struct LedStep {
    bool red = false;
    bool blue = false;
    bool green = false;
    uint32_t durationMs = 0;
};

constexpr LedStep kLedSteps[] = {
    {.red = true, .durationMs = 1000},
    {.green = true, .durationMs = 1000},
    {.blue = true, .durationMs = 1000},
    {.durationMs = 500},
    {.blue = true, .durationMs = 1500},
    {.durationMs = 500},
    {.blue = true, .durationMs = 1500},
    {.green = true, .durationMs = 500},
    {.red = true, .durationMs = 500},
    {.durationMs = 1000},
    {.red = true, .durationMs = 500},
    {.durationMs = 1000},
    {.green = true, .durationMs = 500},
    {.durationMs = 1000},
    {.red = true, .durationMs = 500},
    {.blue = true, .durationMs = 500},
    {.durationMs = 250},
    {.blue = true, .durationMs = 500},
    {.red = true, .durationMs = 500},
    {.green = true, .durationMs = 500},
    {.durationMs = 2000},
};

void setup() {
    pinMode(kLedRed, OUTPUT);
    pinMode(kLedBlue, OUTPUT);
    pinMode(kLedGreen, OUTPUT);
}

void ledState(LedStep step, bool speedTwo) {
    digitalWrite(kLedRed, step.red ? HIGH : LOW);
    digitalWrite(kLedBlue, step.blue ? HIGH : LOW);
    digitalWrite(kLedGreen, step.green ? HIGH : LOW);
    delay(speedTwo ? step.durationMs / kSpeedTwoCoeff : step.durationMs);
}

void loop() {
    static bool speedTwo = false;
    for (const auto& step : kLedSteps) {
        ledState(step, speedTwo);
    }
    speedTwo = !speedTwo;
}