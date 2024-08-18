#pragma once

#include "Page.hpp"
#include "hardware/MotorRegulator.hpp"
#include "cableplotter/PositionController.hpp"

namespace ui {
    namespace build {

        /// Страница управление регулятором мотора
        void motorRegulatorControlPage(Page *p, hardware::MotorRegulator &regulator);

        /// Страница управления позицией
        void positionControlPage(ui::Page *p, cableplotter::PositionController &controller);

        /// Страница выбора файла на печать
//        void selectFilePage(Page *p);

        /// Страница отображаемая во время печати
//        void printingPage(Page *p);
    }
}