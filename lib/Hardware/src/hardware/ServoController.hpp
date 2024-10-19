#pragma once

#include <cstdint>

#include "ESP32Servo.h"


namespace hardware {
    class ServoController {
        mutable Servo servo;
        const uint8_t PIN;

    public:
        explicit ServoController(uint8_t pin);

        void enable() const;

        void disable() const;

        void setPosition(uint8_t angle) const;
    };
} // namespace hardware
