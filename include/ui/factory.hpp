#pragma once

#include "Widget.hpp"

namespace ui {
    Widget *label(const char *title);

    Widget *button(const char *title, std::function<void(Widget *)> callback);

    Widget *display(void *value, ValueType type);

    Widget *spinbox(int *value, int step = 1, std::function<void(Widget *)> on_spin = nullptr);
}