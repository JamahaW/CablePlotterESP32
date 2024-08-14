#include "main.hpp"

#include <Arduino.h>
#include <EncButton.h>

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"

#include "cableplotter/PositionController.hpp"

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

hardware::motor_regulator_config_t regulator_config = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
};

cableplotter::PositionController positionController(
        hardware::MotorRegulator(
                regulator_config,
                hardware::Encoder(PIN_MOTOR_LEFT_ENCODER_A, PIN_MOTOR_LEFT_ENCODER_B),
                hardware::MotorDriverL293(PIN_MOTOR_LEFT_DRIVER_A, PIN_MOTOR_LEFT_DRIVER_B)
        ),
        hardware::MotorRegulator(
                regulator_config,
                hardware::Encoder(PIN_MOTOR_RIGHT_ENCODER_A, PIN_MOTOR_RIGHT_ENCODER_B),
                hardware::MotorDriverL293(PIN_MOTOR_RIGHT_DRIVER_A, PIN_MOTOR_RIGHT_DRIVER_B)
        )
);

gfx::OLED display;
EncButton encoder(PIN_USER_ENCODER_A, PIN_USER_ENCODER_B, PIN_USER_ENCODER_BUTTON);
ui::Window window(display, []() -> ui::Event {
    using ui::Event;
    encoder.tick();
    if (encoder.left()) return Event::NEXT_ITEM;
    if (encoder.right()) return Event::PAST_ITEM;
    if (encoder.click()) return Event::CLICK;
    if (encoder.leftH()) return Event::CHANGE_UP;
    if (encoder.rightH()) return Event::CHANGE_DOWN;
    return Event::IDLE;
});

void vimConfig(ui::Page *p) {
    auto s = ui::label("~");
    for (int i = 0; i < 6; i++) p->addItem(s);
    p->addItem(ui::button(nullptr, [](ui::Widget *w) {
        static bool f = false;
        w->value = (f ^= 1) ? "--[INSERT]--" : "--[NORMAL]--";
    })->unbindFlags(ui::StyleFlag::SQUARE_FRAMED));
}

void motorPageConfig(ui::Page *p, hardware::MotorRegulator &regulator) {
    static ui::Widget *L1 = ui::label("target/delta");
    static ui::Widget *pos_label = ui::label("ticks: ");

    p->addItem(L1);
    p->addItem(ui::spinbox(new int(0), 500, [&regulator](ui::Widget *w) {
        regulator.setTarget(*(int *) w->value);
    }, 50000, -50000));
    p->addItem(ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) {
        regulator.setDelta(short(*(int *) w->value));
    }, regulator_config.d_ticks_max));
    p->addItem(new ui::Group(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
            }
    ));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;

    vimConfig(mainPage.addPage("VIM"));
    motorPageConfig(mainPage.addPage("motor Left"), positionController.left_regulator);
    motorPageConfig(mainPage.addPage("motor Right"), positionController.right_regulator);
}

[[noreturn]] void regulatorUpdateTask(void *) {
    positionController.left_regulator.encoder.attach();
    positionController.right_regulator.encoder.attach();

    while (true) {
        positionController.left_regulator.update();
        positionController.right_regulator.update();
        vTaskDelay(pdMS_TO_TICKS(regulator_config.d_time * 1000));
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}

void setup() {
    analogWriteFrequency(30000);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");
    display.init();
    display.print("HELLO WORLD");

    buildUI();

    delay(100);

    xTaskCreatePinnedToCore(regulatorUpdateTask, "regulators", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update();
}
