#pragma once

#include "Page.hpp"
#include "hardware/MotorRegulator.hpp"
#include "cableplotter/PositionController.hpp"
#include "cableplotter/PaintToolController.hpp"

namespace ui {
    namespace build {
        
        /// Страница для управления устройством печати
        void paintToolControlPage(Page *parent);

        /// Страница управление регулятором мотора
        void motorRegulatorControlPage(Page *p, hardware::MotorRegulator &regulator);

        /// Страница управления позицией
        void positionControlPage(Page *p, cableplotter::PositionController &controller);

        /// Страница управления инструментов печати
        void paintToolControlPage(Page *parent, cableplotter::PaintToolController &paintToolController);
    }
}