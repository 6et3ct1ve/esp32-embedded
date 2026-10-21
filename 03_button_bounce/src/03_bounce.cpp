 #include <Arduino.h>
 #include <atomic>
 #include <cstdint>

 namespace {
   constexpr uint8_t kButtonIn = 16;
   constexpr uint32_t kQuietMs = 50;
   constexpr uint32_t kSerialBaud = 115200;

   std::atomic<uint32_t> buttonCounter{0};

   void IRAM_ATTR buttonIsr() {
     buttonCounter++;
   }
 } // namespace

 void setup() {
   pinMode(kButtonIn, INPUT_PULLUP);
   Serial.begin(kSerialBaud);

   attachInterrupt(digitalPinToInterrupt(kButtonIn), buttonIsr, FALLING);
 }

 void loop() {
  static uint32_t lastChangeMs = millis();
  static uint32_t burstStartCount = 0;
  static uint32_t lastCount = 0;
  const uint32_t now = millis();
  const uint32_t count = buttonCounter.load();

  if (count != lastCount) {
    lastChangeMs = now;
    lastCount = count;
  }

  if (now - lastChangeMs >= kQuietMs && count != burstStartCount) {
    Serial.printf("Button count: %u\n", count - burstStartCount);
    burstStartCount = count;
  }
 }