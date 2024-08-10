#include "ui/Group.hpp"
#include <Arduino.h>

void ui::Group::render(gfx::OLED &display, bool selected) const {
    for (int i = 0; i < widgets.size(); i++) {
        bool lit = (i == cursor) ^ (not control_inner) && selected;
        widgets[i]->render(display, lit);
        display.setInvertText(lit);
        display.write(' ');
    }
    display.setInvertText(false);
}

void ui::Group::onClick() {
    if (control_inner) widgets[cursor]->onClick();
    control_inner ^= 1;
}

void ui::Group::onChange(int change) {
    if (control_inner) {
        widgets[cursor]->onChange(change);
        return;
    }
    cursor = constrain(cursor + change, 0, widgets.size() - 1);
}

ui::Group::Group(const std::vector<Widget *> &widgets) : widgets(widgets) {}