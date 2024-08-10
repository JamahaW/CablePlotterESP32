#pragma once

#include "Widget.hpp"

namespace ui {
    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *display(void *value, ValueType type);

    Widget *spinbox(int *value, int step = 1, void (*on_spin)(Widget &) = nullptr);
}