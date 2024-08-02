#include <Arduino.h>
#include <EncButton.h>
#include <GyverOLED.h>

#include "motor.hpp"
#include "tools.hpp"


// настройки регулятора общие для моторов
motor_regulator_config_t motorRegulatorConfig = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
};

L293NMotor motor(12, 13);
GA25Encoder encoder(14, 27);


MotorRegulator regulator(motorRegulatorConfig, encoder, motor);

class TestManu {
    Button button_up = Button(16);
    Button button_down = Button(17);
    GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;

    enum Modes {
        CHANGE_TARGET = 0,
        CHANGE_DELTA = 1,
        CHANGE_TARGET_STEP,
        LAST
    };

    int mode = Modes::CHANGE_DELTA;
    char step_shift = 0;

    void changeProcess(int8_t factor) {
        switch (mode) {

            case CHANGE_DELTA:
                regulator.setDelta(regulator.getDelta() + factor);
                break;
            case CHANGE_TARGET:
                regulator.setTarget(regulator.getTarget() + factor * calcStep());
                break;

            case CHANGE_TARGET_STEP:
                step_shift += factor;
                step_shift = constrain(step_shift, 1, 16);
                break;

            case LAST:
                break;
        }

    }

public:
    void init() {
        display.init();
        display.clear();
    }

    void update() {
        button_up.tick();
        button_down.tick();

        if (button_up.click() or button_up.step()) {
            changeProcess(1);
        }

        if (button_up.hold()) {
            mode = (mode + 1) % Modes::LAST;
            display.setCursor(0, 0);
            display.printf("mode: %d", mode);
        }

        if (button_down.click() or button_down.step()) {
            changeProcess(-1);
        }

        static plt::Timer info_log_timer(500);
        if (info_log_timer.isReady()) {
            display.setCursor(0, 1);
            display.printf("CurT: %5d\n\r", regulator.encoder.ticks);
            display.printf("Target: %5d\n\r", regulator.getTarget());
            display.printf("DeltaT: %3d\n\r", regulator.getDelta());
            display.printf("isReady: %d\n\r", regulator.isReady());
            display.printf("step: %5d\n\r", calcStep());
        }
    }

    int calcStep() const { return (1 << step_shift); }
} menu;

void setup() {
    analogWriteFrequency(30000); // разогнал ШИМ чтобы не пищал
    Wire.setClock(1000000UL); // разогнал Wire чтоб дисплей не тормозил регулятор

    menu.init();

    regulator.encoder.attach(); // подключаю прерывания энкодера
    regulator.setTarget(1000); // цель регулятора = 1000 тиков энкодера
    regulator.setDelta(12);  // дельта = 12 тиков/dT
}


void loop() {
    regulator.update(); // обновление регулятора
    menu.update();
}



