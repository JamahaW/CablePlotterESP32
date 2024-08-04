#include "gui/Widget.hpp"

gui::Widget::Widget(uint8_t flags, pico::Font font, ValueType type, void *value, void (*onClick)(gui::Widget &),
                    void (*onChange)(gui::Widget &, int)) :
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

void gui::Widget::drawFramed(pico::OLED &display, char begin, char end) const {
    display.write(begin);
    draw(display);
    display.write(end);
}

gui::Widget gui::label(const char *title, pico::Font font) {
    return Widget(
            Widget::StyleFlag::ISOLATED,
            font,
            Widget::ValueType::CHARS,
            (void *) title
    );
}

gui::Widget gui::button(const char *title, void (*callback)(gui::Widget &), pico::Font font) {
    return Widget(
            Widget::StyleFlag::SQUARE_FRAMED | Widget::StyleFlag::ISOLATED,
            font,
            Widget::ValueType::CHARS,
            (void *) title,
            callback
    );
}

gui::Widget gui::display(void *value, gui::Widget::ValueType type, pico::Font font) {
    return Widget(
            Widget::StyleFlag::ISOLATED,
            font,
            type,
            value
    );
}

gui::Widget gui::spinbox(int *value, pico::Font font, void (*on_spin)(Widget &)) {
    return Widget(
            Widget::StyleFlag::TRIANGLE_FRAMED | Widget::StyleFlag::ISOLATED,
            font,
            gui::Widget::ValueType::INT,
            value,
            on_spin,
            [](Widget &w, int c) { *(int *) w.value += c; }
    );
}
