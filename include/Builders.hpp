#pragma once

#include "ui/Page.hpp"

#include "hardware/MotorRegulator.hpp"

#include "cableplotter/PositionController.hpp"
#include "cableplotter/PaintToolController.hpp"

#include "bytelang/StreamInterpreter.hpp"


namespace ui {
    namespace build {

        /// Страница управления устройством печати
        void paintToolControlPage(Page *parent);

        /// Страница управление регулятором мотора
        void motorRegulatorControlPage(Page *p, hardware::MotorRegulator &regulator);

        /// Страница управления позицией
        void positionControlPage(Page *p, cableplotter::PositionController &controller);

        /// Страница управления инструментов печати
        void paintToolControlPage(Page *parent, cableplotter::PaintToolController &paintToolController);

        /// Страница процесса печати
        void printingPage(Page *p, bytelang::StreamInterpreter &interpreter);
    }
}