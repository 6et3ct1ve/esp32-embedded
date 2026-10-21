 #include <Arduino.h>
 #include <atomic>

constexpr uint8_t kButtonIn = 16;

 std::atomic<uint32_t> buttonCounter{0};

 uint32_t lastCount = 0;

 void IRAM_ATTR button_isr() {
   buttonCounter++;
 }

 void setup() {
   pinMode(kButtonIn, INPUT_PULLUP);
   Serial.begin(115200);

   attachInterrupt(digitalPinToInterrupt(kButtonIn), button_isr, FALLING);
 }

 void loop() {
  const uint32_t count = buttonCounter.load();
   if (count != lastCount) {
     lastCount = count;
     Serial.print("Button Pressed! Count: ");
     Serial.println(lastCount);
   }

   delay(10);
 }