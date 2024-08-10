#include <utility>

#include "ui/Widget.hpp"


ui::Widget::Widget(
        uint8_t flags,
        ValueType type,
        void *value,
        std::function<void(Widget *)> on_click,
        std::function<void(Widget *, int)> on_change
) :
        flags(flags),
        type(type),
        value(value),
        on_change(std::move(on_change)),
        on_click(std::move(on_click)) {}

void ui::Widget::render(gfx::OLED &display, bool selected) const {
    display.setFont(font);
    display.setInvertText(selected);

    if (not(flags & StyleFlag::COMPACT) or selected) {
        switch (flags) {
            case StyleFlag::SQUARE_FRAMED:
                drawFramed(display, '[', ']');
                break;
            case StyleFlag::TRIANGLE_FRAMED:
                drawFramed(display, '<', '>');
                break;
            case StyleFlag::ARROW_PREFIX:
                display.print("-> ");
            default:
                draw(display);
                break;
        }

    } else {
        display.write('-');
    }

    display.setInvertText(false);
}

void ui::Widget::onClick() {
    if (on_click != nullptr) on_click(this);
}

void ui::Widget::onChange(int change) {
    if (on_change != nullptr) on_change(this, change);
}

void ui::Widget::draw(gfx::OLED &display) const {
    if (value == nullptr) {
        display.print("null");
        return;
    }

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

void ui::Widget::drawFramed(gfx::OLED &display, char begin, char end) const {
    display.write(begin);
    draw(display);
    display.write(end);
}