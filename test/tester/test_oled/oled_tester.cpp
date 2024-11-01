#include <Wire.h>
#include "Arduino.h"

#include "gfx/OLED.hpp"
#include "ui/Window.hpp"

#include "ui/Factory.hpp"


gfx::OLED display;

ui::Window &window = ui::Window::getInstance(display);


void buildUI(ui::Page &p) {
    static int value_int = 43;
    static float value_float = 0.87;

    p.addItem(ui::label("Label"));

    p.addItem(ui::spinbox(&value_int));

    p.addItem(ui::spinboxF(&value_float, 0.025F));

    p.addItem(ui::display(&value_int, ui::ValueType::INT));

    p.addItem(ui::display(&value_float, ui::ValueType::FLOAT));

    p.addItem(ui::button("reset", [](ui::Widget *) {
        value_int = 0;
        value_float = 0.0F;
    }));
}

void setup() {
    Wire.begin(19, 23);
    display.init();

    buildUI(window.main_page);
}

void loop() {
    window.update();
}
