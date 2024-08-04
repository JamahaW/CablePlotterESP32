#pragma once

#include <vector>
#include "gfx/OLED.hpp"
#include "Widget.hpp"
#include "Page.hpp"
#include "Input.hpp"

namespace ui {

    class Window {

    private:
        gfx::OLED &display;
        const Input &input;

    public:
        static Window *instance;

        Page *page = nullptr;

        explicit Window(gfx::OLED &display, Input &input);

        void update();
    };
}