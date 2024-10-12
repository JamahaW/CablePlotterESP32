#pragma once

#include "Page.hpp"
#include "hardware/MotorRegulator.hpp"
#include "cableplotter/PositionController.hpp"
#include "hardware/ServoController.hpp"

namespace ui {
    namespace build {
        
        /// Страница для управления устройством печати
        void paintToolControlPage(Page *parent);

        /// Страница управление регулятором мотора
        void motorRegulatorControlPage(Page *p, hardware::MotorRegulator &regulator);

        /// Страница управления позицией
        void positionControlPage(Page *p, cableplotter::PositionController &controller);

        /// Страница выбора файла на печать
        // void selectFilePage(Page *p);

        /// Страница отображаемая во время печати
//        void selectFilePage(Page *p, gfx::OLED &display);

        /// Страница управления инструментов печати
        void paintToolControlPage(Page *parent, hardware::ServoController &servoController);
    }
}