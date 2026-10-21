 #include <Arduino.h>
 #include <atomic>

constexpr uint8_t kButtonIn = 16;
constexpr uint32_t kQuietMs = 50;

 std::atomic<uint32_t> buttonCounter{0};

 void IRAM_ATTR buttonIsr() {
   buttonCounter++;
 }

 void setup() {
   pinMode(kButtonIn, INPUT_PULLUP);
   Serial.begin(115200);

   attachInterrupt(digitalPinToInterrupt(kButtonIn), buttonIsr, FALLING);
 }

 void loop() {
  static uint32_t lastChangeMs = millis();
  static uint32_t burstStart = 0;
  static uint32_t lastCount = 0;
  const uint32_t now = millis();
  const uint32_t count = buttonCounter.load();

  if (count != lastCount) {
    lastChangeMs = now;
    lastCount = count;
  }

  if (now - lastChangeMs >= kQuietMs && count != burstStart) {
    Serial.printf("Button count: %u\n", count - burstStart);
    burstStart = count;
  }
 }