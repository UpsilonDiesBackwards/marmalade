#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include "../window.h"

namespace Marmalade::GUI {

    class Log : public Window {
    public:
        explicit Log(bool visible) : Window(visible) {};

        void Draw() override;

    private:
        bool coloredText = true;
        bool autoScroll = true;
    };
}


#endif
