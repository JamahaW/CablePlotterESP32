#include "ui/Window.hpp"

ui::Window::Window(gfx::OLED &display, ui::Input &input) : display(display), input(input) {}

void ui::Window::update() {
    if (page == nullptr) return;

    if (page->handleInput(input)) {
        page->render(display);
    }
}

ui::Widget *ui::Window::pageSetter(ui::Page *target) {
    return new PageSetter(target, this);
}

ui::Window::PageSetter::PageSetter(ui::Page *target, ui::Window *window) :
        ui::Widget(
                ui::ISOLATED | ui::SQUARE_FRAMED,
                ui::ValueType::CHARS,
                (void *) target->title,
                [](ui::Widget &w) {
                    auto &p = (PageSetter & )(w);
                    p.window->page = p.target;
                    p.window->display.clear();
                }
        ),
        target(target),
        window(window) {}