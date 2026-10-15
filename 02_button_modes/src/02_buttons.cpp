#include <Arduino.h>

constexpr uint8_t kAlternateButton = 0;
constexpr uint8_t kRed = 15;
constexpr uint8_t kBlue = 16;
constexpr uint8_t kSyncButton = 21;

struct LedStep {
    bool red = false;
    bool blue = false;
    uint32_t durationMs = 0;
};

void setup() {
    pinMode(kAlternateButton, INPUT);
    pinMode(kRed, OUTPUT);
    pinMode(kBlue, OUTPUT);
    pinMode(kSyncButton, INPUT_PULLUP);
}


void loop() {
    static bool speedTwo = false;
    for (const auto& step : kLedSteps) {
        ledState(step, speedTwo);
    }
    speedTwo = !speedTwo;
}