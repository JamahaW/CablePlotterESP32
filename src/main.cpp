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

ui::Page printing_page(window, "Printing");
static int progress = 0;

bytelang::Result vm_exit(bytelang::Reader &) {
    Serial.printf("vm_exit\n");

    return bytelang::Result::EXIT_OK;
}

bytelang::Result vm_delay(bytelang::Reader &reader) {
    bytelang::u16 duration;
    reader.read(duration);

    delay(duration);

    Serial.printf("vm_delay (u16:%d)\n", duration);
    return bytelang::Result::OK;
}

bytelang::Result vm_set_canvas_size(bytelang::Reader &reader) {
    bytelang::u16 canvas_width;
    bytelang::u16 canvas_height;

    reader.read(canvas_width);
    reader.read(canvas_height);

    positionController.canvas_width = canvas_width;
    positionController.canvas_height = canvas_height;

    Serial.printf("vm_set_canvas_size (i16:%d, i16:%d)\n", canvas_width, canvas_height);

    return bytelang::Result::OK;
}

bytelang::Result vm_set_motors_speed(bytelang::Reader &reader) {
    bytelang::i8 delta_tick;

    reader.read(delta_tick);

    positionController.left_regulator.setDelta(delta_tick);
    positionController.right_regulator.setDelta(delta_tick);

    Serial.printf("vm_set_motors_speed (u8:%d)\n", delta_tick);

    return bytelang::Result::OK;
}

bytelang::Result vm_set_progress(bytelang::Reader &reader) {
    bytelang::u8 v;
    reader.read(v);

    progress = v;

    Serial.printf("vm_set_progress (u8:%d)\n", v);

    return bytelang::Result::OK;
}

bytelang::Result vm_set_speed_multiplication(bytelang::Reader &reader) {
    bytelang::u16 multiplication;

    reader.read(multiplication);

    Serial.printf("set_speed_multiplication (u16:%d)\n", multiplication);

    return bytelang::Result::OK;
}

bytelang::Result vm_move_to(bytelang::Reader &reader) {
    bytelang::i16 x;
    bytelang::i16 y;

    reader.read(x);
    reader.read(y);

    positionController.setTarget(x, y);

    Serial.printf("vm_move_to (i16:%d, i16:%d)\n", x, y);

    return bytelang::Result::OK;
}

bytelang::StreamInterpreter<7> interpreter(
        {
                vm_exit,
                vm_delay,
                vm_set_canvas_size,
                vm_set_motors_speed,
                vm_set_progress,
                vm_set_speed_multiplication,
                vm_move_to,
        });

void ui_printing(ui::Page *p) {
    p->addItem(new ui::Group({ui::label("Progress"), ui::display(&progress, ui::ValueType::INT)}));

    p->addItem(ui::button("PAUSE", [](ui::Widget *w) {
        static bool p = true;
        interpreter.setPaused(p ^= 1);
        w->value = (void *) (p ? "RESUME" : "PAUSE");
    }));

    p->addItem(ui::button("ABORT", [](ui::Widget *) {
        interpreter.abort();
    }));

    p->addPage("Tune");
}

void ui_motor(ui::Page *p, hardware::MotorRegulator &regulator) {
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

fs::File bytecode_stream;

void ui_positionRegulator(ui::Page *p) {
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

[[noreturn]] void printing_task(void *) {
    window.setPage(&printing_page);

    bytelang::Result result = interpreter.run(bytecode_stream);

    bytecode_stream.close();

    Serial.printf("result: %hhd", static_cast<char>(result));

    window.setPage(&window.main_page);
    vTaskDelete(nullptr);

    while (true) {}
}

void startPrinting(ui::Widget *widget) {
    String path = '/' + String((const char *) widget->value);
    bytecode_stream = SD.open(path);

    if (not bytecode_stream) {
        Serial.printf("Bytecode stream not opened. F: %s", path.c_str());
        return;
    }

    printing_page.title = (const char *) widget->value;

    xTaskCreate(
            printing_task,
            "printing",
            4096,
            nullptr,
            1,
            nullptr
    );

    Serial.printf("Executing: '%s'", path.c_str());
}

void ui_print_select_file(ui::Page *p) {
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
                p->addItem(new ui::FileWidget(file, startPrinting));
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
    ui_print_select_file(mainPage.addPage("PRINT"));
    ui_positionRegulator(mainPage.addPage("PositionController"));
    ui_motor(mainPage.addPage("motor Left"), positionController.left_regulator);
    ui_motor(mainPage.addPage("motor Right"), positionController.right_regulator);
    ui_printing(&printing_page);
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
        delay(uint32_t(regulator_config.d_time * 1000));
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}


void setup() {
    analogWriteFrequency(30000);
    Serial.begin(9600);
    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    display.init();
    Serial.println("HELLO WORLD");

    buildUI();

    xTaskCreatePinnedToCore(regulatorUpdateTask, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update(false);
}
