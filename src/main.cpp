#include <Arduino.h>
#include <EncButton.h>
#include <Wire.h>

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"

// настройки регулятора общие для моторов
hardware::motor_regulator_config_t regulator_config = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
};

hardware::MotorRegulator regulatorLeft(regulator_config, hardware::Encoder(18, 19), hardware::MotorDriverL293(33, 25));
hardware::MotorRegulator regulatorRight(regulator_config, hardware::Encoder(16, 17), hardware::MotorDriverL293(26, 27));


gfx::OLED display;
EncButton encoder(34, 35, 32);
ui::Window window(display, []() -> ui::Event {
    encoder.tick();
    if (encoder.left()) return ui::Event::NEXT;
    if (encoder.right()) return ui::Event::PAST;
    if (encoder.click()) return ui::Event::CLICK;
    if (encoder.leftH()) return ui::Event::CHANGE_UP;
    if (encoder.rightH()) return ui::Event::CHANGE_DOWN;
    return ui::Event::IDLE;
});

void calcConfig(ui::Page *p) {
    static int a = 0, b = 0, res = 0;
    auto re_calc = [](ui::Widget &) { res = a * b; };

    auto *w = new ui::WidgetGroup(
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
        hardware::MotorRegulator &regulator,
        void (*on_target_update)(ui::Widget &),
        void (*on_delta_update)(ui::Widget &)
) {
    static ui::Widget *L1 = ui::label("target/delta");
    static ui::Widget *pos_label = ui::label("ticks: ");

    p->addItem(L1);
    p->addItem(ui::spinbox(new int(0), 2000, on_target_update));
    p->addItem(ui::spinbox(new int(0), 1, on_delta_update));
    p->addItem(new ui::WidgetGroup(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
            }
    ));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;
    calcConfig(mainPage.addPage("calculator"));

    auto f_t_left = [](ui::Widget &w) { regulatorLeft.setTarget(*(int *) w.value); };
    auto f_d_left = [](ui::Widget &w) { regulatorLeft.setDelta(short(*(int *) w.value)); };
    motorPageConfig(mainPage.addPage("motor Left"), regulatorLeft, f_t_left, f_d_left);
}

TaskHandle_t Task1;

[[noreturn]] void taskCode(void *) {
    while (true) {
        Serial.println("task1");
    }
}

void setup() {
    analogWriteFrequency(30000);
    regulatorLeft.encoder.attach();
    regulatorRight.encoder.attach();
    Serial.begin(9600);
    Wire.setClock(1000000UL);
    display.init();

    buildUI();

    display.print("HELLO WORLD");

    xTaskCreatePinnedToCore(
            taskCode,
            "w",
            4096,
            nullptr,
            1,
            &Task1,
            1
    );

//    xTaskCreatePinnedToCore(
//            [](void *) {
//                window.update();
//            },
//            "ui",
//            4096,
//            nullptr,
//                1,
//            nullptr,
//            1
//    );
}

void loop() {
    regulatorLeft.update();
    regulatorRight.update();
}



