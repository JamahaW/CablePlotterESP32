#include <Arduino.h>
#include <EncButton.h>
#include <GyverOLED.h>

#include "motor.hpp"
#include "tools.hpp"

Button button_up(16);
Button button_down(17);


GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;


bool motor_mode_flag = true;
GA25Encoder encoder(14, 27);
L293NMotor motor(12, 13);

motor_regulator_state_t regulator_state = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
};

MotorRegulator regulator(regulator_state, encoder, motor);


void setup() {
    analogWriteFrequency(30000);
    Serial.begin(9600);
    encoder.attach();
    display.init();
    display.clear();
    Wire.setClock(1000000UL);

    regulator.target_ticks = 1000;
    regulator.delta_ticks = 15;
}


void updateMotorValue(int8_t factor) {
    if (motor_mode_flag) {
        regulator.delta_ticks += factor;
        regulator.delta_ticks = constrain(regulator.delta_ticks, 1, regulator_state.d_ticks_max);
    } else {
        regulator.target_ticks += factor * 1000;
    }
}

void loop() {
    button_up.tick();
    button_down.tick();
    regulator.update();

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
        display.printf("CurT: %5d\n\r", encoder.ticks);
        display.printf("Target: %5d\n\r", regulator.target_ticks);
        display.printf("DeltaT: %3d\n\r", regulator.delta_ticks);
    }
}



