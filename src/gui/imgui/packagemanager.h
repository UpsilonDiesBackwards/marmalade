#ifndef MARMALADE_PACKAGEMANAGER_H
#define MARMALADE_PACKAGEMANAGER_H

#include "../window.h"

#include <string>


namespace Marmalade::GUI {

    enum PackageManagerTab {
        PackageManagerTab_ALL,
        PackageManagerTab_INSTALLED,
        PackageManagerTab_AVAILABLE
    };

    class PackageManager : public Window {
    public:
        void Draw() override;

    private:
        float progress{0};
        bool progressIndeterminate{false};
        std::string progressText{};

        void drawLeftPane(PackageManagerTab tab);
        void drawRightPane(PackageManagerTab tab);
        void drawSplit(PackageManagerTab tab, float bottom_bar_height);
        void drawBottomBar(float height);

        std::string itemId(const char* id, PackageManagerTab tab);
        void clone_repo();
    };
}


#endif
