#include "main.hpp"

#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <EncButton.h>

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"

#include "cableplotter/PositionController.hpp"

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

#include "bytelang/StreamInterpreter.hpp"
#include "bytelang/Types.hpp"

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

bytelang::Result vm_exit(bytelang::Reader &) {
    Serial.printf("vm_exit\n");
    return bytelang::Result::EXIT_OK;
}

bytelang::Result vm_delay(bytelang::Reader &reader) {
    bytelang::u16 duration;

    reader.read(duration);

    Serial.printf("vm_delay (u16:%d)\n", duration);
    return bytelang::Result::OK;
}

bytelang::Result vm_set_motors_speed(bytelang::Reader &reader) {
    bytelang::u8 delta_tick;

    reader.read(delta_tick);

    Serial.printf("vm_set_motors_speed (u8:%d)\n", delta_tick);
    return bytelang::Result::OK;
}

bytelang::Result vm_move_to(bytelang::Reader &reader) {
    bytelang::i16 target_x;
    bytelang::i16 target_y;

    reader.read(target_x);
    reader.read(target_y);

    Serial.printf("vm_move_to (i16:%d, i16:%d)\n", target_x, target_y);
    return bytelang::Result::OK;
}

bytelang::StreamInterpreter<4> interpreter(
        {
                vm_exit,
                vm_delay,
                vm_set_motors_speed,
                vm_move_to,
        });

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

void motorPageConfig(ui::Page *p, hardware::MotorRegulator &regulator) {
    static ui::Widget *L1 = ui::label("target/delta");
    static ui::Widget *pos_label = ui::label("ticks: ");

    p->addItem(L1);

    p->addItem(ui::spinbox(new int(0), 500, [&regulator](ui::Widget *w) {
        regulator.target = (*(int *) w->value);
    }, 50000, -50000));

    p->addItem(ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) {
        regulator.setDelta(char(*(int *) w->value));
    }, regulator_config.d_ticks_max));

    p->addItem(new ui::Group(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
            }
    ));
}

ui::Item *makeVector2iSetter(ui::Page *p, const char *title, ui::Widget *x_spinbox, ui::Widget *y_spinbox) {
    static ui::Widget *x_label = ui::label("x:");
    static ui::Widget *y_label = ui::label("y:");
    p->addItem(ui::label(title));
    return new ui::Group({x_label, x_spinbox, y_label, y_spinbox});
}

ui::Widget *makePositionSpinbox(int *value) {
    constexpr int STEP = 50, MAX_DIST_MM = 1500;
    return ui::spinbox(value, STEP, nullptr, MAX_DIST_MM, -MAX_DIST_MM);
}

void positionRegulatorPageConfig(ui::Page *p) {
    static int target_x = 0, target_y = 0;

    auto update_position = [](ui::Widget *) { positionController.setTarget(target_x, target_y); };

    p->addItem(makeVector2iSetter(
            p, "target",
            makePositionSpinbox(&target_x),
            makePositionSpinbox(&target_y)
    ));

    p->addItem(ui::button("run", update_position));

    ui::Widget *spinbox = ui::spinbox(new int(5), 1, [](ui::Widget *w) {
        auto v = char(*(int *) (w->value));
        positionController.left_regulator.setDelta(v);
        positionController.right_regulator.setDelta(v);
    }, regulator_config.d_ticks_max);

    p->addItem(new ui::Group({ui::label("delta"), spinbox}));

    p->addItem(makeVector2iSetter(
            p, "delta L-X, R-Y",
            ui::spinbox(&positionController.left_offset, 5, update_position, 200, -200),
            ui::spinbox(&positionController.right_offset, 5, update_position, 200, -200)
    ));

    p->addItem(ui::button("reset ticks", [](ui::Widget *) {
        positionController.right_regulator.reset();
        positionController.left_regulator.reset();
    }));

    p->addItem(makeVector2iSetter(
            p, "canvas",
            makePositionSpinbox(&positionController.canvas_width),
            makePositionSpinbox(&positionController.canvas_height)
    ));

    p->addItem(ui::spinboxF(&positionController.ticks_in_mm, 5, 10000));
}

void start_printing(ui::Widget *widget) {

    String path = '/' + String((const char *) widget->value);

    fs::File bytecode_stream = SD.open(path);

    if (not bytecode_stream) {
        Serial.printf("Bytecode stream not opened. F: %s", path.c_str());
        return;
    }

    Serial.printf("Executing: '%s'", path.c_str());

    interpreter.run(bytecode_stream);

    bytecode_stream.close();
}

void sdPageConfig(ui::Page *p) {
    p->addItem(ui::button("reload", [p](ui::Widget *w) {
        display.clear();
        p->clearItems();
        p->addItem(w);

        if (not SD.begin(PIN_SD_CS)) {
            display.print("SD init not success");
            return;
        }

        fs::File root = SD.open("/");

        if (not root) {
            display.print("Root open error");
            return;
        }

        fs::File file = root.openNextFile();

        while (file) {
            if (not file.isDirectory()) {
                p->addItem(new ui::FileWidget(file, start_printing));
            }

            file.close();
            file = root.openNextFile();
        }

        file.close();
        root.close();
    }));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;
    positionRegulatorPageConfig(mainPage.addPage("PositionController"));
    motorPageConfig(mainPage.addPage("motor Left"), positionController.left_regulator);
    motorPageConfig(mainPage.addPage("motor Right"), positionController.right_regulator);
    sdPageConfig(mainPage.addPage("PRINT"));
}

[[noreturn]] void regulatorUpdateTask(void *) {
    positionController.left_regulator.encoder.attach();
    positionController.right_regulator.encoder.attach();
    positionController.left_regulator.setDelta(8);
    positionController.right_regulator.setDelta(8);
    positionController.ticks_in_mm = CONST_TICKS_IN_MM;
    positionController.canvas_height = 1200;
    positionController.canvas_width = 1200;

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

    display.init();

    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    buildUI();

    xTaskCreatePinnedToCore(regulatorUpdateTask, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update();
}
