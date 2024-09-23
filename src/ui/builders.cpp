#include "ui/builders.hpp"
#include "ui/Group.hpp"
#include "ui/factory.hpp"


void ui::build::motorRegulatorControlPage(Page *p, hardware::MotorRegulator &regulator) {
    /*
     [    Размер дисплея    ]

     Regulator-L
     >Main
     dT:{<17>}
     POS: 9999 / {<10000>}
     [reset ticks]
     [move 0]
     [[ S T O P ]]
     */

    constexpr int MOTOR_MAX_ABS_POS = 50000;

    static Widget *speed_label = ui::label("Speed: ");
    static Widget *pos_label = ui::label("POS:");
    static Widget *pos_delimiter_label = ui::label("/");

    p->addItem(new ui::Group(
            {
                    speed_label,
                    ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) {
                        regulator.setDelta(*(char *) w->value);
                    }, regulator.config.d_ticks_max)
            }));

    p->addItem(new ui::Group(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
                    pos_delimiter_label,
                    ui::spinbox(new int(0), 1000, [&regulator](ui::Widget *w) {
                        regulator.target = *(int *) w->value;
                    }, MOTOR_MAX_ABS_POS, -MOTOR_MAX_ABS_POS)
            }));

    p->addItem(ui::button("reset", [&regulator](ui::Widget *) {
        regulator.reset();
    }));

    p->addItem(ui::button("move 0", [&regulator](ui::Widget *) {
        regulator.target = 0;
    }));

    p->addItem(ui::button("STOP", [&regulator](ui::Widget *) {
        regulator.target = regulator.encoder.ticks;
        regulator.setDelta(0);
        regulator.motor.set(0);
    }));
}

static ui::Group *makeNamedDoubleGroup(const char *name, ui::Widget *first, ui::Widget *second) {
    return new ui::Group({ui::label(name), first, second});
}

static ui::Widget *makeSpinbox(int *value) {
    constexpr int MM_STEP = 25;
    constexpr int MAX_POS = 2000;
    return ui::spinbox(value, MM_STEP, nullptr, MAX_POS, -MAX_POS);
}

static ui::Widget *makeOffsetSpinbox(int *value, std::function<void(ui::Widget *)> &&on_spin = nullptr) {
    constexpr int STEP = 5;
    constexpr int ABS_MAX_OFFSET = 100;
    return ui::spinbox(value, STEP, on_spin, ABS_MAX_OFFSET, -ABS_MAX_OFFSET);
}

void ui::build::positionControlPage(ui::Page *p, cableplotter::PositionController &controller) {
/*
 [----Размер-дисплея----]

 PositionController
 >Main
 {XY:} <1111> <2222>
 [[ m o v e ]]
 {Canvas:} <1200> <1200>
 {OffsetLR:} <255> <255>
 */

    static int target_x = 0;
    static int target_y = 0;

    auto update_position = [&controller](ui::Widget *) {
        controller.setTarget(target_x, target_y);
    };

    p->addItem(makeNamedDoubleGroup("XY:", makeSpinbox(&target_x), makeSpinbox(&target_y)));

    p->addItem(ui::button("move", update_position));

    p->addItem(makeNamedDoubleGroup("Canvas:",
                                    makeSpinbox(&controller.canvas_width),
                                    makeSpinbox(&controller.canvas_height)));

    p->addItem(makeNamedDoubleGroup("OffsetLR:",
                                    makeOffsetSpinbox(&controller.left_offset, update_position),
                                    makeOffsetSpinbox(&controller.right_offset, update_position)
    ));

}


