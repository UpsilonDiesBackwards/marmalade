#ifndef ENGINE_TOPBAR_H
#define ENGINE_TOPBAR_H


#include "../../gui/stylemanager.h"
#include "../../application/profiler.h"
#include "packagemanager.h"
#include "log.h"
#include "../window.h"

#include <vector>

class TopBar {
public:
    inline TopBar() {
        windows.push_back(&packageManager);
        windows.push_back(&log);
    }

    void Show();
private:
    Marmalade::GUI::PackageManager packageManager{};
    Marmalade::GUI::Log log{true};
    bool showDebugWindow = false;

    std::vector<Marmalade::GUI::Window*> windows{};
};

#endif //ENGINE_TOPBAR_H
