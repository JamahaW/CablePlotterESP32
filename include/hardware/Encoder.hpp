#pragma once

#include <cstdint>

namespace hardware {
    class GA25Encoder {

    public:
        const uint8_t PIN_A;
        const uint8_t PIN_B;

        /// Текущая позиция в тиках
        volatile mutable long ticks = 0;

        explicit GA25Encoder(uint8_t a, uint8_t b);

        /// Подключить обработчик прерывания
        void attach() const;
    };
}