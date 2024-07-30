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



//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

//#include <GyverOLED.h>
//extern L293NMotor m;
//
//void IRAM_ATTR motor_enc() { m._onEncoderInterrupt(); }
//
//L293NMotor m(12, 13, 18, 21, motor_enc);

#define PIN_ENC_A 14//18
#define PIN_ENC_B 27//21

Encoder e(PIN_ENC_A, PIN_ENC_B);

void setup() {
    Serial.begin(9600);

    e.init();

//    analogWriteFrequency(30000);

}

void loop() {
    Serial.println(e.ticks);
}
