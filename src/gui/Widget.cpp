#include "gui/Widget.hpp"

gui::Widget::Widget(
        uint8_t flags,
        pico::Font font,
        Widget::ValueType type,
        void *value,
        void (*onChange)(Widget &, int),
        void (*onClick)(Widget &)) :
        flags(flags),
        font(font),
        type(type),
        value(value),
        on_change(onChange),
        on_click(onClick) {}

void gui::Widget::render(pico::OLED &display, bool selected) const {
    display.setFont(font);
    display.setInvertText(selected);

    if (not(flags & StyleFlag::COMPACT) or selected) {
        if (flags & StyleFlag::SQUARE_FRAMED) {
            drawFramed(display, '[', ']');
        } else if (flags & StyleFlag::TRIANGLE_FRAMED) {
            drawFramed(display, '<', '>');
        } else {
            draw(display);
        }
    } else {
        display.write('-');
    }

    display.setInvertText(false);
    display.write((flags & StyleFlag::ISOLATED) ? '\n' : ' ');
}

void gui::Widget::onClick() {
    if (on_click != nullptr) on_click(*this);
}

void gui::Widget::onChange(int change) {
    if (on_change != nullptr) on_change(*this, change);
}

void gui::Widget::draw(pico::OLED &display) const {
    switch (type) {
        case ValueType::CHARS:
            display.print((const char *) value);
            return;

        case ValueType::INT:
            display.print(*(int *) value);
            return;

        case ValueType::FLOAT:
            display.print(*(float *) value);
            return;
    }
}

void gui::Widget::drawFramed(pico::OLED &display, char begin, char end) const {
    display.write(begin);
    draw(display);
    display.write(end);
}

gui::Widget gui::label(const char *title, pico::Font font, uint8_t flags) {
    return Widget(flags, font, Widget::ValueType::CHARS, (void *) title);
}
