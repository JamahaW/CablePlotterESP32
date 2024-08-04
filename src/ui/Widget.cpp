#include <cstdint>
#include "ui/Widget.hpp"

ui::Widget::Widget(uint8_t flags, ValueType type, void *value, void (*onClick)(ui::Widget &),
                   void (*onChange)(ui::Widget &, int), int16_t config) :
        flags(flags),
        type(type),
        value(value),
        on_change(onChange),
        on_click(onClick),
        config(config) {}

void ui::Widget::render(gfx::OLED &display, bool selected) const {
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

void ui::Widget::onClick() {
    if (on_click != nullptr) on_click(*this);
}

void ui::Widget::onChange(int change) {
    if (on_change != nullptr) on_change(*this, change);
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

ui::Widget *ui::label(const char *title) {
    return new Widget(
            StyleFlag::ISOLATED,
            ValueType::CHARS,
            (void *) title);
}

ui::Widget *ui::button(const char *title, void (*callback)(ui::Widget &)) {
    return new Widget(
            StyleFlag::SQUARE_FRAMED | StyleFlag::ISOLATED,
            ValueType::CHARS,
            (void *) title,
            callback);
}

//ui::Widget *ui::pageSetter(ui::Page *page) {
//
//    return new Widget(
//            StyleFlag::ISOLATED, ValueType::CHARS, (void *) page->title, [&page](Widget &w) {
//                Window::instance->page = page;
//            }
//    );
//}

ui::Widget *ui::display(void *value, ui::ValueType type) {
    return new Widget(StyleFlag::ISOLATED, type, value);
}

ui::Widget *ui::spinbox(int &value, int step, void (*on_spin)(ui::Widget &)) {
    return new Widget(
            StyleFlag::TRIANGLE_FRAMED | StyleFlag::ISOLATED,
            ValueType::INT,
            &value,
            on_spin,
            [](Widget &w, int c) { *(int *) w.value += c * w.config; }, int16_t(step)
    );
}
