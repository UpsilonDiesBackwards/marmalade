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

#include "window.h"
#include "imgui/packagemanager.h"
#include "imgui/log.h"
#include "imgui/projectwizard.h"
#include "imgui/preferences.h"
#include "imgui/settings.h"
#include "imgui/projectbrowser.h"
#include "imgui/about.h"

#include <vector>

namespace Marmalade::GUI {
    class WindowManager {
    public:
        static WindowManager& GetInstance();

        PackageManager packageManager{};
        Log log{true};
        ProjectWizard projectWizard{};
        Preferences preferences{};
        ProjectSettings settings{};
        ProjectBrowser projectBrowser{true};
        About about{};

        std::vector<Window*> windows{};

        bool showDebugWindow = false;

        void ToggleDebugWindow();
    private:
        WindowManager();
    };
}


#endif
