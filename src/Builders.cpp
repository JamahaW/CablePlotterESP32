#include "Builders.hpp"
#include "ui/Group.hpp"
#include "ui/Factory.hpp"


#define STR(x) (#x)

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

    p->addItem(new ui::Group({speed_label, ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) { regulator.setDelta(*(char *) w->value); }, regulator.config.delta_ticks_max)}));

    p->addItem(new ui::Group({pos_label, ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT), pos_delimiter_label,
                              ui::spinbox(new int(0), 10, [&regulator](ui::Widget *w) { regulator.setTarget(*(int *) w->value); }, MOTOR_MAX_ABS_POS, -MOTOR_MAX_ABS_POS)}));

    p->addItem(ui::button("reset", [&regulator](ui::Widget *) { regulator.reset(); }));

    p->addItem(ui::button("move 0", [&regulator](ui::Widget *) { regulator.target_ticks = 0; }));

    p->addItem(ui::button("STOP", [&regulator](ui::Widget *) {
        regulator.target_ticks = regulator.encoder.ticks;
        regulator.setDelta(0);
        regulator.motor.set(0);
    }));
}


static ui::Group *makeNamedDoubleGroup(const char *name, ui::Widget *first, ui::Widget *second) {
    return new ui::Group({ui::label(name), first, second}, 1);
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

    p->addItem(makeNamedDoubleGroup("Canvas:", makeSpinbox(&controller.canvas_width), makeSpinbox(&controller.canvas_height)));

    p->addItem(makeNamedDoubleGroup("OffsetLR:", makeOffsetSpinbox(&controller.left_regulator.offset_mm, update_position), makeOffsetSpinbox(&controller.right_regulator.offset_mm, update_position)));

    p->addItem(ui::button("set home", [&controller](ui::Widget *) {
        long distance_right;
        long distance_left;

        controller.calcDistance(distance_left, distance_right, 0, 0);

        controller.left_regulator.setCurrent(distance_left);
        controller.left_regulator.setTarget(distance_left);

        controller.right_regulator.setCurrent(distance_right);
        controller.right_regulator.setTarget(distance_right);
    }));

    p->addItem(ui::button("disable toggle", [&controller](ui::Widget *) { controller.regulators_disable ^= 1; }));
}

static ui::Group *paintToolSetupWidget(
        const char *name, cableplotter::PaintToolController::Tool tool, cableplotter::PaintToolController &paintToolController
) {
    static constexpr int TOOL_STEP = 5;

    return new ui::Group({ui::label(name), ui::spinbox(&paintToolController.positions[tool], TOOL_STEP, [&paintToolController](ui::Widget *widget) {
        paintToolController.servo_controller.setPosition(*(int *) (widget->value));
    }, 180)}, 1);
}

#define PAINT_TOOL_WIDGET(tool, controller) (paintToolSetupWidget(STR(tool), tool, controller))

void ui::build::paintToolControlPage(ui::Page *parent, cableplotter::PaintToolController &paintToolController) {
    using Tool = cableplotter::PaintToolController::Tool;

    parent->addItem(PAINT_TOOL_WIDGET(Tool::NONE, paintToolController));
    parent->addItem(PAINT_TOOL_WIDGET(Tool::LEFT, paintToolController));
    parent->addItem(PAINT_TOOL_WIDGET(Tool::MIDDLE, paintToolController));
    parent->addItem(PAINT_TOOL_WIDGET(Tool::RIGHT, paintToolController));

    parent->addItem(ui::button("Disable", [&paintToolController](ui::Widget *w) { paintToolController.servo_controller.disable(); }));
    parent->addItem(ui::button("Enable", [&paintToolController](ui::Widget *w) { paintToolController.servo_controller.enable(); }));
}

void ui::build::printingPage(ui::Page *p, bytelang::StreamInterpreter &interpreter) {
    p->addItem(ui::button("PAUSE", [&interpreter](ui::Widget *w) {
        static bool p = false;
        interpreter.setPaused(p ^= 1);
        w->value = (void *) (p ? "RESUME" : "PAUSE");
    }));

    p->addItem(ui::button("ABORT", [&interpreter](ui::Widget *) {
        interpreter.abort();
    }));

    p->addPage("Tune");
}
