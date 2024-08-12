#include "main.hpp"

#include <Arduino.h>
#include <EncButton.h>
#include <Wire.h>

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"


hardware::motor_regulator_config_t regulator_config = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
};

hardware::MotorRegulator regulatorLeft(
        regulator_config,
        hardware::Encoder(PIN_MOTOR_LEFT_ENCODER_A, PIN_MOTOR_LEFT_ENCODER_B),
        hardware::MotorDriverL293(PIN_MOTOR_LEFT_DRIVER_A, PIN_MOTOR_LEFT_DRIVER_B)
);

hardware::MotorRegulator regulatorRight(
        regulator_config,
        hardware::Encoder(PIN_MOTOR_RIGHT_ENCODER_A, PIN_MOTOR_RIGHT_ENCODER_B),
        hardware::MotorDriverL293(PIN_MOTOR_RIGHT_DRIVER_A, PIN_MOTOR_RIGHT_DRIVER_B)
);

gfx::OLED display;
EncButton encoder(PIN_USER_ENCODER_A, PIN_USER_ENCODER_B, PIN_USER_ENCODER_BUTTON);
ui::Window window(display, []() -> ui::Event {
    using ui::Event;
    encoder.tick();
    if (encoder.left()) return Event::NEXT;
    if (encoder.right()) return Event::PAST;
    if (encoder.click()) return Event::CLICK;
    if (encoder.leftH()) return Event::CHANGE_UP;
    if (encoder.rightH()) return Event::CHANGE_DOWN;
    return Event::IDLE;
});

void calcConfig(ui::Page *p) {
    static int a = 0, b = 0, res = 0;
    auto re_calc = [](ui::Widget *) { res = a * b; };
    auto *w = new ui::Group(
            {
                    ui::spinbox(&a, 1, re_calc),
                    ui::label(" * "),
                    ui::spinbox(&b, 1, re_calc),
                    ui::label(" = "),
                    ui::display(&res, ui::ValueType::INT)
            });
    p->addItem(w);
}

void motorPageConfig(
        ui::Page *p,
        hardware::MotorRegulator &regulator
) {
    static ui::Widget *L1 = ui::label("target/delta");
    static ui::Widget *pos_label = ui::label("ticks: ");

    p->addItem(L1);
    p->addItem(ui::spinbox(new int(0), 1000, [&regulator](ui::Widget *w) {
        regulator.setTarget(*(int *) w->value);
    }));
    p->addItem(ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) {
        regulator.setDelta(short(*(int *) w->value));
    }));
    p->addItem(new ui::Group(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
            }
    ));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;
    calcConfig(mainPage.addPage("calculator"));

    motorPageConfig(mainPage.addPage("motor Left"), regulatorLeft);
    motorPageConfig(mainPage.addPage("motor Right"), regulatorRight);
}


void setup() {
    analogWriteFrequency(30000);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");
    display.init();
    display.print("HELLO WORLD");

    buildUI();

    xTaskCreatePinnedToCore(
            [](void *) {
                regulatorLeft.encoder.attach();
                regulatorRight.encoder.attach();
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
                while (true) {
                    regulatorLeft.update();
                    regulatorRight.update();
                }
#pragma clang diagnostic pop
            },
            "regs",
            configMINIMAL_STACK_SIZE,
            nullptr,
            0,
            nullptr,
            0
    );
}

void loop() {
    window.update();
}
