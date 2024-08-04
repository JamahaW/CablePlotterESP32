#include "ui/Page.hpp"

ui::Page::Page(const char *title) : title(title) {}

void ui::Page::render(gfx::OLED &display) const {
    display.setCursor(0, 0);
    display.println(title);
    for (auto i = 0; i < widgets.size(); i++) {
        widgets[i]->render(display, i == cursor);
    }
    display.clearAfterCursor();
}

bool ui::Page::handleInput(const ui::Input &input) {
    switch (input.getEvent()) {
        case Input::Event::IDLE:
            return false;

        case Input::Event::CLICK:
            widgets[cursor]->onClick();
            return true;

        case Input::Event::NEXT:
            moveCursor(1);
            return true;

        case Input::Event::PAST:
            moveCursor(-1);
            return true;

        case Input::Event::CHANGE_UP:
            widgets[cursor]->onChange(1);
            return true;

        case Input::Event::CHANGE_DOWN:
            widgets[cursor]->onChange(-1);
            return true;
    }
    return false;
}

void ui::Page::moveCursor(int delta) {
    cursor = constrain(cursor + delta, 0, widgets.size() - 1);
}
