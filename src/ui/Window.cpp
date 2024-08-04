#include "ui/Window.hpp"

ui::Window *ui::Window::instance = nullptr;

ui::Window::Window(gfx::OLED &display, ui::Input &input) : display(display), input(input) {
    instance = this;
}

void ui::Window::update() {
    if (page == nullptr) return;

    if (page->handleInput(input)) {
        page->render(display);
    }
}
