#pragma once

#include <vector>
#include "Widget.hpp"
#include "gfx/OLED.hpp"
#include "Input.hpp"


namespace ui {
    class Page {

    private:
        int cursor = 0;

    public:
        const char *title;

        std::vector<Widget *> widgets;

        explicit Page(const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(const Input &input);

    private:

        void moveCursor(int delta);

    };
}