#include "tools/Timer.hpp"

tools::Timer::Timer(uint16_t period) : period(period) {}

bool tools::Timer::isReady() const {
    unsigned long current = millis();

    if (current < next_ms) {
        return false;
    }

    next_ms = current + period;
    return true;
}

void tools::Timer::setPeriod(unsigned short new_period) {
    period = new_period;
}
