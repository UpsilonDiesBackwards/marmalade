#ifndef ENGINE_TOPBAR_H
#define ENGINE_TOPBAR_H


#include "../../gui/stylemanager.h"
#include "../../application/profiler.h"
#include "packagemanager.h"

class TopBar {
public:
    void Show();
private:
    bool showPackageManager = false;
    PackageManager packageManager;
};


#endif //ENGINE_TOPBAR_H
