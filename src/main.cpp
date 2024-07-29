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



void motor_test(const Motor &m, int pwm, bool reverse, int duration);

void setup() {
    analogWriteFrequency(30000);

    Motor m(13, 12, 14);

    for (int i = 0; i < 256; i += 16) {
        motor_test(m, i, false, 1000);
    }
}

void motor_test(const Motor &m, int pwm, bool reverse, int duration) {
    m.setPWM(pwm);
    m.setDirection(reverse);
    delay(duration);
    m.setPWM(0);
    delay(duration / 4);
}


void loop() {
}
