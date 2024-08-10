#include <utility>

#include "ui/factory.hpp"

ui::Widget *ui::button(const char *title, std::function<void(Widget *)> callback) {
    return new Widget(
            StyleFlag::SQUARE_FRAMED,
            ValueType::CHARS,
            (void *) title,
            std::move(callback));
}

ui::Widget *ui::display(void *value, ValueType type) {
    return new Widget(0, type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ValueType::CHARS);
}

ui::Widget *ui::spinbox(int *value, int step, std::function<void(Widget *)> on_spin) {
    return new Widget(
            StyleFlag::TRIANGLE_FRAMED,
            ValueType::INT,
            value,
            std::move(on_spin),
            [step](Widget *w, int c) {
                *(int *) w->value += c * step;
                w->onClick();
            }
    );
}