#ifndef ENGINE_TOPBAR_H
#define ENGINE_TOPBAR_H


#include "../../gui/stylemanager.h"
#include "../../application/profiler.h"
#include "packagemanager.h"
#include "log.h"

class TopBar {
public:
    void Show();
private:
    bool showPackageManager = false;
    PackageManager packageManager;
    bool showDebugWindow = false;
    Log log;
};

#endif //ENGINE_TOPBAR_H
