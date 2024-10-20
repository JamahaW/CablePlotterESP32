#include "FileWidget.hpp"


static char *c_str_copy(const char *s) {
    char *ret = new char[strlen(s) + 1];
    strcpy(ret, s);
    return ret;
}

ui::FileWidget::FileWidget(
        File &file,
        std::function<void(Widget *)> onClick
) : Widget(
        ValueType::CHARS,
        (void *) c_str_copy(file.name()),
        std::move(onClick),
        nullptr
) {
}

ui::FileWidget::~FileWidget() {
    delete[] (char *) value;
}
