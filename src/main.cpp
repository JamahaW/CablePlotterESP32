#include <Arduino.h>
#include <EncButton.h>
#include <GyverOLED.h>

#include "motor.hpp"
#include "tools.hpp"

Button button_up(16);
Button button_down(17);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;

// режим меню
bool motor_mode_flag = true;

// настройки регулятора общие для моторов
motor_regulator_config_t motorRegulatorConfig = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
};

MotorRegulator regulator(motorRegulatorConfig, GA25Encoder(14, 27), L293NMotor(12, 13));


void setup() {
    analogWriteFrequency(30000); // разогнал ШИМ чтобы не пищал
    Serial.begin(9600);
    regulator.encoder.attach(); // подключаю прерывания энкодера
    display.init();
    display.clear();
    Wire.setClock(1000000UL); // разогнал Wire чтоб дисплей не тормозил регулятор

    regulator.target = 1000; // цель регулятора = 1000 тиков энкодера
    regulator.delta = 15;  // дельта = 15 тиков/dT
}


void updateMotorValue(int8_t factor) {
    if (motor_mode_flag) {
        regulator.delta += factor;
        regulator.delta = constrain(regulator.delta, 1, motorRegulatorConfig.d_ticks_max);
    } else {
        regulator.target += factor * 1000;
    }
}

void loop() {
    regulator.update(); // обновление регулятора

    button_up.tick();
    button_down.tick();

    if (button_up.click() or button_up.step()) {
        updateMotorValue(1);
    }

    if (button_up.hold()) {
        motor_mode_flag ^= 1;
        display.setCursor(0, 0);
        display.printf("mode: %d", motor_mode_flag);
    }

    if (button_down.click() or button_down.step()) {
        updateMotorValue(-1);
    }

    static plt::Timer info_log_timer(500);
    if (info_log_timer.ready()) {
        display.setCursor(0, 1);
        display.printf("CurT: %5d\n\r", regulator.encoder.ticks);
        display.printf("Target: %5d\n\r", regulator.target);
        display.printf("DeltaT: %3d\n\r", regulator.delta);
    }
}



