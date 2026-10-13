#include <Arduino.h>

constexpr uint8_t LED_RED = 15;
constexpr uint8_t LED_BLUE = 16;
constexpr uint8_t LED_GREEN = 17;
constexpr uint32_t SPEED_TWO_DURATION = 2;

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
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
}

void ledState(LedStep step, bool speedTwo) {
    digitalWrite(LED_RED, step.red ? HIGH : LOW);
    digitalWrite(LED_BLUE, step.blue ? HIGH : LOW);
    digitalWrite(LED_GREEN, step.green ? HIGH : LOW);
    delay(speedTwo ? step.durationMs / SPEED_TWO_DURATION : step.durationMs);
}

void loop() {
    static bool speedTwo = false;
    for (const auto& step : kLedSteps) {
        ledState(step, speedTwo);
    }
    speedTwo = !speedTwo;
}