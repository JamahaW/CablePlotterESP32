#pragma once

#include <vector>
#include "gfx/OLED.hpp"
#include "Widget.hpp"
#include "Page.hpp"
#include "Input.hpp"


namespace ui {

    class Window {

    private:
        const Input &input;

        class PageSetter : public ui::Widget {
            ui::Page *target;
            Window *window;

        public:
            explicit PageSetter(ui::Page *target, Window *window);

        };

    public:
        gfx::OLED &display;
        Page *page = nullptr;

        explicit Window(gfx::OLED &display, Input &input);

        void update();

        Widget *pageSetter(Page *target);
    };
}