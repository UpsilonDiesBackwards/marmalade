#ifndef MARMALADE_GUI_SETTINGS_H
#define MARMALADE_GUI_SETTINGS_H

#include "../window.h"

namespace Marmalade::GUI {
    class Settings : public Window {
    public:
        void Draw() override;
    private:
        void requiresRestartWarning();
        static bool getLogLevels(void* data, int idx, const char** outText);
    };
}


#endif
