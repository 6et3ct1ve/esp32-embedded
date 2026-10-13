#include <Arduino.h>

constexpr uint8_t LED_RED = 15;
constexpr uint8_t LED_BLUE = 16;
constexpr uint8_t LED_GREEN = 17;

struct LedStep {
    bool red = false;
    bool blue = false;
    bool green = false;
    uint32_t durationMs = 0;
};

constexpr LedStep kLedSteps[] = {
    {.red = true, .blue = false, .green = false, .durationMs = 1000},
    {.red = false, .blue = false, .green = true, .durationMs = 1000},
    {.red = false, .blue = true, .green = false, .durationMs = 1000},
    {.red = false, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = true, .green = false, .durationMs = 1500},
    {.red = false, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = true, .green = false, .durationMs = 1500},
    {.red = false, .blue = false, .green = true, .durationMs = 500},
    {.red = true, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = false, .green = false, .durationMs = 1000},
    {.red = true, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = false, .green = false, .durationMs = 1000},
    {.red = false, .blue = false, .green = true, .durationMs = 500},
    {.red = false, .blue = false, .green = false, .durationMs = 1000},
    {.red = true, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = true, .green = false, .durationMs = 500},
    {.red = false, .blue = false, .green = false, .durationMs = 250},
    {.red = false, .blue = true, .green = false, .durationMs = 500},
    {.red = true, .blue = false, .green = false, .durationMs = 500},
    {.red = false, .blue = false, .green = true, .durationMs = 500},
    {.red = false, .blue = false, .green = false, .durationMs = 2000},
};

void setup() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
}

void ledState(LedStep step) {
    digitalWrite(LED_RED, step.red ? HIGH : LOW);
    digitalWrite(LED_BLUE, step.blue ? HIGH : LOW);
    digitalWrite(LED_GREEN, step.green ? HIGH : LOW);
    delay(step.durationMs);
}

void loop() {
    for (const auto& step : kLedSteps) {
        ledState(step);
    }
}