#include "ui/Window.hpp"


ui::Window::Window(gfx::OLED &display, std::function<Event()>&& input_handler) :
        display(display),
        main_page(*this, "Main"),
        current_page(&main_page),
        input_handler(input_handler) {}

void ui::Window::update(bool force_update) {
    if (current_page->handleInput(input_handler()) or force_update) {
        current_page->render(display);
    }
}


