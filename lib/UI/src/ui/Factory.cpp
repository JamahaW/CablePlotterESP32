#include <utility>
#include <Arduino.h>

#include <ui/Factory.hpp>


ui::Widget *ui::button(const char *title, std::function<void(Widget *)> callback) {
    return (new Widget(
            ValueType::CHARS,
            (void *) title,
            std::move(callback))
    )->setStyle(ui::Style::SQUARE_FRAMED);
}

ui::Widget *ui::display(void *value, ValueType type) {
    return new Widget(type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ValueType::CHARS);
}

ui::Widget *ui::spinbox(int *value, int step, std::function<void(Widget *)> on_spin, int max, int min) {
    return (new Widget(
            ValueType::INT,
            value,
            std::move(on_spin),
            [step, max, min](Widget *w, int c) {
                *(int *) w->value = constrain(*(int *) w->value + c * step, min, max);
                w->onClick();
            }
    )
    )->setStyle(ui::Style::TRIANGLE_FRAMED);
}

ui::Widget *ui::spinboxF(float *value, float step, float max, float min, std::function<void(Widget *)> on_spin) {
    return (new Widget(
            ValueType::FLOAT,
            value,
            std::move(on_spin),
            [step, max, min](Widget *w, int c) {
                *(float *) w->value = constrain(*(float *) w->value + c * step, min, max);
                w->onClick();
            }
    ))->setStyle(ui::Style::TRIANGLE_FRAMED);
}


