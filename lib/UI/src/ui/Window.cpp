#include <ui/Window.hpp>


ui::Window::Window(gfx::OLED &display, std::function<Event()> &&input_handler) :
        display(display), main_page(*this, "Main"), current_page(&main_page), input_handler(input_handler) {
}

void ui::Window::update() {
    if (current_page->handleInput(input_handler())) {
        current_page->render(display);
    }
}

void ui::Window::setPage(ui::Page *new_page) {
    current_page = new_page;
    display.clear();
    update();
}

ui::Window &ui::Window::getInstance(gfx::OLED &display) {
    static Window window(display, []() -> ui::Event {
        using ui::Event;

        static EncButton encoder(constants::USER_ENCODER_A, constants::USER_ENCODER_B, constants::USER_ENCODER_BUTTON);

        encoder.tick();

        if (encoder.left()) {
            return Event::NEXT_ITEM;
        }
        if (encoder.right()) {
            return Event::PAST_ITEM;
        }
        if (encoder.click()) {
            return Event::CLICK;
        }
        if (encoder.leftH()) {
            return Event::CHANGE_UP;
        }
        if (encoder.rightH()) {
            return Event::CHANGE_DOWN;
        }

        return Event::IDLE;
    });
    return window;
}


