#pragma once

#include <vector>

#include <ui/Item.hpp>
#include <ui/Widget.hpp>


namespace ui {
    class Group : public Item {

    private:
        bool control_inner = true;
        int cursor;
        std::vector<Widget *> widgets;

    public:
        explicit Group(const std::vector<Widget *> &widgets, int init_cursor_position = 0);

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;
    };
}