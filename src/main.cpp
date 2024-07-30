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
//extern L293NMotorEncoder m;
//
//void IRAM_ATTR motor_enc() { m._onEncoderInterrupt(); }
//
//L293NMotorEncoder m(12, 13, 18, 21, motor_enc);

#define PIN_ENC_A 14//18
#define PIN_ENC_B 27//21

class Encoder {

public:

    const uint8_t PIN_A;
    const uint8_t PIN_B;

    volatile int32_t ticks = 0;

    Encoder(uint8_t a, uint8_t b);

    void init();

};

void IRAM_ATTR encoder_int(void *v) {
    auto *e = (Encoder *) v;

    if (digitalRead(e->PIN_B)) {
        e->ticks++;
    } else {
        e->ticks--;
    }
}

Encoder::Encoder(uint8_t a, uint8_t b) : PIN_B(b), PIN_A(a) {
    pinMode(a, INPUT);
    pinMode(b, INPUT);
}

void Encoder::init() {
    attachInterruptArg(PIN_A, encoder_int, this, RISING);
}

Encoder e(PIN_ENC_A, PIN_ENC_B);

void setup() {
    Serial.begin(9600);

    e.init();

//    analogWriteFrequency(30000);

}

void loop() {
    Serial.println(e.ticks);
}
