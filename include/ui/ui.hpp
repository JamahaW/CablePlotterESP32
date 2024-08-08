#pragma once

#include <vector>
#include <esp_bit_defs.h>
#include "gfx/OLED.hpp"

namespace ui {
    enum StyleFlag : char {
        /// Виджет будет скрываться вне фокуса
        COMPACT = BIT(1),
        /// Обрамление []
        SQUARE_FRAMED = BIT(2),
        /// Обрамление <>
        TRIANGLE_FRAMED = BIT(3),
    };

    enum class ValueType : char {
        CHARS,
        INT,
        FLOAT,
    };

    /// Элемент, который отображается на странице
    class Item {
    public:
        virtual void render(gfx::OLED &display, bool selected) const = 0;

        virtual void onClick() = 0;

        virtual void onChange(int change) = 0;
    };

    class Widget : public Item {
    private:
        using OnClickHandler = void(Widget &);
        using OnChangeHandler = void(Widget &, int);

        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandler *const on_click;
        OnChangeHandler *const on_change;

    public:
        int16_t config;

        const void *value;

        explicit Widget(uint8_t flags, ValueType type, void *value, OnClickHandler *on_click = nullptr,
                        OnChangeHandler *on_change = nullptr, int16_t config = 0);

        Widget *bindFlags(uint8_t on_flags) {
            flags |= on_flags;
            return this;
        }

        Widget *unbindFlags(uint8_t off_flags) {
            flags &= ~off_flags;
            return this;
        }

        Widget *setFont(gfx::Font new_font) {
            font = new_font;
            return this;
        }

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;

    private:
        void draw(gfx::OLED &display) const;

        void drawFramed(gfx::OLED &display, char begin, char end) const;

    };

    class WidgetGroup : public Item {

    private:
        bool control_inner = false;
        int cursor = 0;
        std::vector<Widget *> widgets;

    public:
        explicit WidgetGroup(const std::vector<Widget *> &widgets);

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;
    };

    class Window;

    enum class Event : char {
        /// Ничего не произошло
        IDLE,
        /// Клик на виджет
        CLICK,
        /// Выбрать следующий виджет
        NEXT,
        /// Выбрать предыдущий виджет
        PAST,
        /// Изменение вверх
        CHANGE_UP,
        /// Изменение вниз
        CHANGE_DOWN,
    };

    class Page {

    private:

        class PageSetter : public Widget {
            Page *target;
            Window &window;

        public:
            explicit PageSetter(Page *target, Window &window);
        };

        int cursor = 0;
        Window &window;
        std::vector<Item *> items;
        const char *title;
        PageSetter to_this_page;

    public:
        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(Event e);

        Page *addPage(const char *child_title);

        void addItem(Item *w);


    private:

        void moveCursor(int delta);

    };

    class Window {

    private:
        using EventHandler = Event();
        EventHandler *const input;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page;

        explicit Window(gfx::OLED &display, Event (*input)());

        void update();
    };

    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *display(void *value, ui::ValueType type);

    Widget *spinbox(int &value, int step = 1, void (*on_spin)(Widget &) = nullptr);
}