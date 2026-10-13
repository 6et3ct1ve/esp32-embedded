#include <Arduino.h>

// Визначаємо цифровий пін, до якого підключимо зовнішній світлodіод.
// Суфікс _RED одразу підказує, що цей пін працює на вихід.
constexpr uint8_t LED_RED = 15;
constexpr uint8_t LED_BLUE = 16;

void setup() {
    // Налаштовуємо пін як вихід (OUTPUT), щоб контролер міг подавати на нього напругу
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void loop() {
    digitalWrite(LED_RED, HIGH); // Вмикаємо світлodіod (подаємо 3.3 В)
    digitalWrite(LED_BLUE, LOW); // Вимикаємо світлdіod (подаємо 0 В)
    delay(1000);                 // Чекаємо 1 секунду (1000 мілісекунд)
    
    digitalWrite(LED_RED, LOW);  // Вимикаємо світлdіod (подаємо 0 В)
    digitalWrite(LED_BLUE, HIGH); // Вмикаємо світлdіod (подаємо 3.3 В)
    delay(1000);                 // Чекаємо 1 секунду
}