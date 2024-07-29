#include <Arduino.h>
#include "motor.hpp"

//#include <EncButton.h>
//#include "listeners.hpp"
//
//
//Button button_up(16);
//Button button_down(17);
//
//ButtonListener up_listener("Up", &button_up);
//ButtonListener down_listener("Down", &button_down);
//
//void setup() {
//    Serial.begin(9600);
//    Serial.println(F("Start!"));
//    pinMode(LED_BUILTIN, OUTPUT);
//}
//
//void loop() {
//    button_up.tick();
//    button_down.tick();
//    up_listener.update();
//    down_listener.update();
//
//    static bool f = false;
//    f ^= button_up.click() || button_down.click();
//
//    digitalWrite(LED_BUILTIN, f);
//}



//#include <GyverOLED.h>
//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;



void setup() {
    Serial.begin(9600);
    analogWriteFrequency(30000);

    Motor m(12, 13, 0, 0, nullptr);
    delay(1200);

    m.set(120);
    delay(10000);

    m.set(-120);
    delay(10000);

    m.set(0);
}


void loop() {
}
