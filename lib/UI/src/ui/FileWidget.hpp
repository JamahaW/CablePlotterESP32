#pragma once

#include <FS.h>

#include <ui/Widget.hpp>


namespace ui {
    class FileWidget : public Widget {

    public:
        explicit FileWidget(fs::File &file, std::function<void(Widget *)> onClick);

        ~FileWidget();
    };
}

