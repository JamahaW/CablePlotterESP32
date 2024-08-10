#include "ui/Window.hpp"
#include <Arduino.h>


ui::Window::Window(gfx::OLED &display, std::function<Event()>&& input_handler) :
        display(display),
        main_page(*this, "Main"),
        current_page(&main_page),
        input_handler(input_handler) {}

void ui::Window::update() {
    if (current_page->handleInput(input_handler())) {
        current_page->render(display);
    }
}


