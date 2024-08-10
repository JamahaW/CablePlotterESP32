#include "ui/factory.hpp"

ui::Widget *ui::button(const char *title, void (*callback)(Widget &)) {
    return new Widget(
            StyleFlag::SQUARE_FRAMED,
            ValueType::CHARS,
            (void *) title,
            callback);
}

ui::Widget *ui::display(void *value, ValueType type) {
    return new Widget(0, type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ValueType::CHARS);
}

ui::Widget *ui::spinbox(int *value, int step, void (*on_spin)(Widget &)) {
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