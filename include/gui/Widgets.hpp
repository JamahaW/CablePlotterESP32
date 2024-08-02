#pragma once

#include "pico/OLED.hpp"

namespace gui {

    /// Базовый виджет
    class Widget {
    public:
        /// Отрисовать виджет на странице
        virtual void render(pico::OLED &display, bool selected) const = 0;

        /// Событие клика на виджет
        virtual void onClick() {}

        /// Событие изменения (для кнопок и ползунков)
        virtual void onChange() {}
    };

    /// Отображение текста
    class Label : public Widget {
    private:
        const char *title;
        const pico::Font font;

    public:
        explicit Label(const char *title, pico::Font font = pico::Font::SINGLE) : title(title), font(font) {}

        void render(pico::OLED &display, bool selected) const override {
            display.setFont(font);
            display.setInvertText(selected);
            display.println(title);
            display.setInvertText(false);
        }
    };

    /// Кнопка с обработчиком событий
    class Button : public Widget {
    private:
        const char *title;
        /// Поместить кнопку на этой же строке
        bool merge;

        void (*callback)();

    public:
        explicit Button(const char *title, bool merge = false, void (*callback)() = nullptr) :
                title(title),
                callback(callback),
                merge(merge) {}

        void render(pico::OLED &display, bool selected) const override {
            display.setFont(pico::Font::SINGLE);
            display.setInvertText(selected);
            if (merge) display.write('\n');
            display.write('[');
            display.print(title);
            display.write(']');
            display.setInvertText(false);
        }

        void onClick() override {
            if (callback != nullptr) callback();
        }
    };
}