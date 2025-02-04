#ifndef MARMALADE_GUI_TOPBAR_H
#define MARMALADE_GUI_TOPBAR_H

#include "../window.h"
#include "packagemanager.h"
#include "log.h"
#include "projectwizard.h"
#include "settings.h"

#include <vector>

namespace Marmalade::GUI {

    class TopBar {
    public:
        inline TopBar() {
            windows.push_back(&packageManager);
            windows.push_back(&log);
            windows.push_back(&projectWizard);
            windows.push_back(&settings);
        }

        void Show();

    private:
        Marmalade::GUI::PackageManager packageManager{};
        Marmalade::GUI::Log log{true};
        Marmalade::GUI::ProjectWizard projectWizard{};
        Marmalade::GUI::Settings settings{};

        bool showDebugWindow = false;

        std::vector<Marmalade::GUI::Window*> windows{};
    };
}

#endif
