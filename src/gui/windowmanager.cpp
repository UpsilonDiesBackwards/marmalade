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

#include "windowmanager.h"

Marmalade::GUI::WindowManager& Marmalade::GUI::WindowManager::GetInstance() {
    static WindowManager instance{};
    return instance;
}

Marmalade::GUI::WindowManager::WindowManager() {
    windows.push_back(&welcomeScreen);
    windows.push_back(&animationManager);
    windows.push_back(&packageManager);
    windows.push_back(&log);
    windows.push_back(&projectWizard);
    windows.push_back(&preferences);
    windows.push_back(&settings);
    windows.push_back(&projectBrowser);
    windows.push_back(&versionControl);
    windows.push_back(&about);
    windows.push_back(&configErrorDlg);
    windows.push_back(&saveWorkspaceDlg);
    windows.push_back(&saveWorkspaceAsDlg);
}

void Marmalade::GUI::WindowManager::ToggleDebugWindow() {
    showDebugWindow = !showDebugWindow;
}
