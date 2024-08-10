#include <Arduino.h>
#include "ui/Item.hpp"

ui::Widget::Widget(uint8_t flags, ValueType type, void *value, void (*on_click)(Widget &),
                   void (*on_change)(Widget &, int), int16_t config) :
        flags(flags),
        type(type),
        value(value),
        on_change(on_change),
        on_click(on_click),
        config(config) {}

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

void ui::WidgetGroup::render(gfx::OLED &display, bool selected) const {
    for (int i = 0; i < widgets.size(); i++) {
        bool lit = (i == cursor) ^ (not control_inner) && selected;
        widgets[i]->render(display, lit);
        display.setInvertText(lit);
        display.write(' ');
    }
    display.setInvertText(false);
}

void ui::WidgetGroup::onClick() {
    if (control_inner) widgets[cursor]->onClick();
    control_inner ^= 1;
}

void ui::WidgetGroup::onChange(int change) {
    if (control_inner) {
        widgets[cursor]->onChange(change);
        return;
    }
    cursor = constrain(cursor + change, 0, widgets.size() - 1);
}

ui::WidgetGroup::WidgetGroup(const std::vector<Widget *> &widgets) : widgets(widgets) {}

ui::Widget *ui::button(const char *title, void (*callback)(ui::Widget &)) {
    return new Widget(
            StyleFlag::SQUARE_FRAMED,
            ValueType::CHARS,
            (void *) title,
            callback);
}

ui::Widget *ui::display(void *value, ui::ValueType type) {
    return new Widget(0, type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ui::ValueType::CHARS);
}

ui::Widget *ui::spinbox(int *value, int step, void (*on_spin)(ui::Widget &)) {
    return new Widget(
            StyleFlag::TRIANGLE_FRAMED,
            ValueType::INT,
            value,
            on_spin,
            [](Widget &w, int c) {
                *(int *) w.value += c * w.config;
                w.onClick();
            }, int16_t(step)
    );
}