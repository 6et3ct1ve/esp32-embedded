#include <Arduino.h>

constexpr uint8_t kAlternateButton = 0;
constexpr uint8_t kRed = 15;
constexpr uint8_t kBlue = 16;
constexpr uint8_t kSyncButton = 21;

constexpr uint32_t kSyncDurationMs = 200;
constexpr uint32_t kAlternateDurationMs = 1000;

struct LedStep {
    bool red = false;
    bool blue = false;
    uint32_t durationMs = 0;
};


constexpr LedStep kSyncSteps[] = {
    {.red = true, .blue = true, .durationMs = kSyncDurationMs},
    {.durationMs = kSyncDurationMs},
};

constexpr LedStep kAlternateSteps[] = {
    {.red = true, .durationMs = kAlternateDurationMs},
    {.blue = true, .durationMs = kAlternateDurationMs},
};

void setup() {
    pinMode(kAlternateButton, INPUT);
    pinMode(kRed, OUTPUT);
    pinMode(kBlue, OUTPUT);
    pinMode(kSyncButton, INPUT_PULLUP);
}

void ledStep(const LedStep& step) {
    digitalWrite(kRed, step.red ? HIGH : LOW);
    digitalWrite(kBlue, step.blue ? HIGH : LOW);
    delay(step.durationMs);
}

void loop() {
    static bool sync = true;
    if (digitalRead(kSyncButton) == LOW) {
        sync = true;
    }
    else if (digitalRead(kAlternateButton) == LOW) {
        sync = false;
    }


    if (sync) {
        for (const auto& step : kSyncSteps) {
            ledStep(step);
        }
    }
    else {
        for (const auto& step : kAlternateSteps) {
            ledStep(step);
        }
    }
}