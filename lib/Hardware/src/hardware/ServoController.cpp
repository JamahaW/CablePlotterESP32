#include <hardware/ServoController.hpp>


hardware::ServoController::ServoController(uint8_t pin) : PIN{pin} {
}

void hardware::ServoController::enable() const {
    servo.attach(PIN);
}

void hardware::ServoController::disable() const {
    servo.detach();
}

void hardware::ServoController::setPosition(uint8_t angle) const {
    servo.write(angle);
}
