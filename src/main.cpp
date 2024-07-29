#include <Arduino.h>
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

struct Motor {
    const uint8_t PIN_SPEED;
    const uint8_t PIN_DIR_A;
    const uint8_t PIN_DIR_B;

    Motor(uint8_t speed, uint8_t dir_a, uint8_t dir_b) : PIN_SPEED(speed), PIN_DIR_A(dir_a), PIN_DIR_B(dir_b) {
        pinMode(PIN_SPEED, OUTPUT);
        pinMode(PIN_DIR_A, OUTPUT);
        pinMode(PIN_DIR_B, OUTPUT);
    }

    void setDirection(bool reverse) const {
        digitalWrite(PIN_DIR_A, reverse);
        digitalWrite(PIN_DIR_B, !reverse);
    }

    void setPWM(uint16_t pwm) const {
        analogWrite(PIN_SPEED, constrain(pwm, 0, 255));
    }

    void set(int16_t value) const {
        setDirection(value > 0);
        setPWM(abs(value));
    }
};

void setup() {
    Motor m(13, 12, 14);

    const int DURATION = 400;

    for (int i = 0; i < 255; i += 10) {
        m.setPWM(i);

        m.setDirection(false);
        delay(DURATION);

        m.setDirection(true);
        delay(DURATION);

        m.setPWM(0);
    }


}

void loop() {
}
