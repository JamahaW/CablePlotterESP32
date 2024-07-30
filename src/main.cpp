#include <Arduino.h>
#include "motor.hpp"

#include <EncButton.h>

Button button_up(16);
Button button_down(17);


#define DELTA 20
//ButtonListener up_listener("Up", &button_up);
//
//#include "listeners.hpp"
//ButtonListener down_listener("Down", &button_down);
//


//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

//#include <GyverOLED.h>

GA25Encoder e(14, 27);
L293NMotor m(12, 13);

void updateMotorPWM(int16_t pwm_dir);

void setup() {
    analogWriteFrequency(30000);
    Serial.begin(9600);
    e.attach();

    m.set(0);
}

void loop() {
    static int16_t pwm_dir = 0;

    button_up.tick();
    if (button_up.click() or button_up.step()) {
        pwm_dir += DELTA;
        updateMotorPWM(pwm_dir);
    }

    button_down.tick();
    if (button_down.click() or button_down.step()) {
        pwm_dir -= DELTA;
        updateMotorPWM(pwm_dir);
    }


    static uint32_t t = 0;

    if (millis() - t > 100) {
        t = millis();
        Serial.println(e.ticks);
    }

}

void updateMotorPWM(int16_t pwm_dir) {
    Serial.printf("pwm=%d\n", pwm_dir);
    m.set(pwm_dir);
}
