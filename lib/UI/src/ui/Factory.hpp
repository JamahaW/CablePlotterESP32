#pragma once

#include <ui/Widget.hpp>


namespace ui {
    Widget *label(const char *title);

    Widget *button(const char *title, std::function<void(Widget *)> callback);

    Widget *display(void *value, ValueType type);

    Widget *spinbox(
            int *value,
            int step = 1,
            std::function<void(Widget *)> on_spin = nullptr,
            int max = 100,
            int min = 0
    );

    Widget *spinboxF(
            float *value,
            float step = 0.1F,
            float max = 1.0F,
            float min = 0.0F,
            std::function<void(Widget *)> on_spin = nullptr
    );
}