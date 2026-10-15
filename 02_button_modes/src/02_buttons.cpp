#include <Arduino.h>

constexpr uint8_t kRed = 15;
constexpr uint8_t kBlue = 16;
constexpr uint32_t kSpeedTwoCoeff = 2;

struct LedStep {
    bool red = false;
    bool blue = false;
    uint32_t durationMs = 0;
};

void setup() {
    pinMode(kRed, OUTPUT);
    pinMode(kBlue, OUTPUT);
}


void loop() {
    static bool speedTwo = false;
    for (const auto& step : kLedSteps) {
        ledState(step, speedTwo);
    }
    speedTwo = !speedTwo;
}