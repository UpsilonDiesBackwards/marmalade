/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_GUI_TOPBAR_H
#define MARMALADE_GUI_TOPBAR_H

#include "../window.h"
#include "packagemanager.h"
#include "log.h"
#include "projectwizard.h"
#include "preferences.h"
#include "settings.h"
#include "projectbrowser.h"
#include "about.h"

#include <vector>

namespace Marmalade::GUI {

    class TopBar {
    public:
        inline TopBar() {
            windows.push_back(&packageManager);
            windows.push_back(&log);
            windows.push_back(&projectWizard);
            windows.push_back(&preferences);
            windows.push_back(&settings);
            windows.push_back(&projectBrowser);
            windows.push_back(&about);
        }

        void Show();

    private:
        Marmalade::GUI::PackageManager packageManager{};
        Marmalade::GUI::Log log{true};
        Marmalade::GUI::ProjectWizard projectWizard{};
        Marmalade::GUI::Preferences preferences{};
        Marmalade::GUI::ProjectSettings settings{};
        Marmalade::GUI::ProjectBrowser projectBrowser{true};
        Marmalade::GUI::About about{};

        bool showDebugWindow = false;

        std::vector<Marmalade::GUI::Window*> windows{};
    };
}

#endif
