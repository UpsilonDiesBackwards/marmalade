/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_GUI_WINDOWMANAGER_H
#define MARMALADE_GUI_WINDOWMANAGER_H

#include "../application/config/engineconfig.h"
#include "window.h"
#include "dialog.h"

#include "windows/welcomescreen.h"
#include "windows/packagemanager.h"
#include "windows/animation.h"
#include "windows/versioncontrol.h"

#include "log/log.h"

#include "wizards/projectwizard.h"

#include "settings/preferences.h"
#include "settings/settings.h"

#include "editor/projectbrowser.h"

#include "dialogs/about.h"
#include "dialogs/configerror.h"
#include "dialogs/saveworkspace.h"
#include "windows/animation.h"
#include "windows/versioncontrol.h"

#include <vector>

// Forward decl
namespace IGFD {
    struct FileDialogConfig;
}
typedef int ImGuiFileDialogFlags;
const int FDF_Modal = 1 << 9;

namespace Marmalade::GUI {
    class WindowManager {
    public:
        struct FileDialogResult {
            std::string FilePath;
        };

        static WindowManager& GetInstance();

        Marmalade::GUI::WelcomeScreen welcomeScreen{Marmalade::EngineConfig::GetStoredConfig().appearance.showWelcomeScreen};

        Animation animationManager{};
        PackageManager packageManager{};
        Log log{true};
        ProjectWizard projectWizard{};
        Preferences preferences{};
        ProjectSettings settings{};
        ProjectBrowser projectBrowser{true};
        VersionControl versionControl{};
        About about{};
        ConfigErrorDialog configErrorDlg{};

        std::vector<Marmalade::GUI::Window*> windows{};
        std::vector<Dialog> dialogs{};
        std::vector<Dialog> fileDialogs{};

        bool showDebugWindow = false;

        void ToggleDebugWindow();

        Dialog RegisterDialog(std::shared_ptr<CustomDialog> customDlg, const std::function<void(bool, void*)>& callback, bool reregister = false, ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse, ImVec2 minSize = ImVec2(800, 500));
        void ShowDialog(std::string name);

        static IGFD::FileDialogConfig PrepareFileDialogConfig(const std::string& path = "", int flags = FDF_Modal);
        Dialog RegisterFileDialog(std::string name, const std::function<void(bool, void*)>& callback, ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse, ImVec2 minSize = ImVec2(800, 500));

    private:
        WindowManager();
    };
}


#endif
