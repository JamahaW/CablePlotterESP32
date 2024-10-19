#include <Arduino.h>

#include <ui/Group.hpp>


void ui::Group::render(gfx::OLED &display, bool selected) const {
    bool lit, item_selected;

    for (int i = 0; i < widgets.size(); i++) {
        item_selected = i == cursor;
        lit = item_selected ^ (not control_inner) and selected;

        display.setInvertText(lit);
        display.write(item_selected * '{');

        widgets[i]->render(display, lit);

        display.setInvertText(lit);
        display.write(item_selected * '}');
        display.write(' ');
    }
    display.setInvertText(false);
}

void ui::Group::onClick() {
    if (control_inner) { widgets[cursor]->onClick(); }
    control_inner ^= 1;
}

void ui::Group::onChange(int change) {
    if (control_inner) {
        widgets[cursor]->onChange(change);
        return;
    }
    cursor = constrain(cursor + change, 0, widgets.size() - 1);
}

ui::Group::Group(const std::vector<Widget *> &widgets, int init_cursor_position) : cursor(init_cursor_position), widgets(widgets) {
}