#pragma once

#include "ui/Widget.hpp"

#include <FS.h>


namespace ui {
    class FileWidget : public Widget {

    public:
        explicit FileWidget(fs::File &file, std::function<void(Widget *)> onClick);

        ~FileWidget();
    };
}

