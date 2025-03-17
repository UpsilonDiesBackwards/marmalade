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
#include "windows/welcomescreen.h"
#include "windows/packagemanager.h"
#include "log/log.h"
#include "wizards/projectwizard.h"
#include "settings/preferences.h"
#include "settings/settings.h"
#include "editor/projectbrowser.h"
#include "dialogs/about.h"
#include "dialogs/configerror.h"

#include <vector>

namespace Marmalade::GUI {
    class WindowManager {
    public:
        static WindowManager& GetInstance();

        WelcomeScreen welcomeScreen{EngineConfig::GetStoredConfig().appearance.showWelcomeScreen};
        PackageManager packageManager{};
        Log log{true};
        ProjectWizard projectWizard{};
        Preferences preferences{};
        ProjectSettings settings{};
        ProjectBrowser projectBrowser{true};
        About about{};
        ConfigErrorDialog configErrorDlg{};

        std::vector<Window*> windows{};

        bool showDebugWindow = false;

        void ToggleDebugWindow();

    private:
        WindowManager();
    };
}


#endif
