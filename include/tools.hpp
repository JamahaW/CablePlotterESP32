//
// Created by NewTToNN on 31.07.2024.
//

#include <Arduino.h>

#ifndef ESP32_TEST_TOOLS_HPP
#define ESP32_TEST_TOOLS_HPP

namespace plt {

    class Timer {
    private:
        uint16_t period;
        mutable uint32_t next_ms = 0;

    public:
        explicit Timer(uint16_t period) : period(period) {}

        bool isReady() const {
            if (millis() - next_ms < period) return false;
            next_ms = millis();
            return true;
        }

        void setPeriod(uint16_t period) { this->period = period; }
    };

} // plt

#endif //ESP32_TEST_TOOLS_HPP
