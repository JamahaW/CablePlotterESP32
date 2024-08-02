#pragma once

#include <Arduino.h>

namespace tools {

    class Timer {
    private:
        unsigned short period;
        mutable unsigned long next_ms = 0;

    public:
        explicit Timer(unsigned short period);

        bool isReady() const;

        void setPeriod(unsigned short new_period);
    };

} // tools
