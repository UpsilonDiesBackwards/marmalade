#ifndef MARMALADE_SETTINGS_H
#define MARMALADE_SETTINGS_H

#include "../window.h"
#include <string>

namespace Marmalade::GUI {
    class ProjectSettings : public Window {
    public:
        void Draw() override;
    private:
        void drawProjectSettings();
        void drawBuildSettings();
        void drawDebugSettings();
    };
}


#endif//MARMALADE_SETTINGS_H
