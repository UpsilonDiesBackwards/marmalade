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

#include "../application/application.h"

#include <ImGuiFileDialog.h>

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
}

void Marmalade::GUI::WindowManager::ToggleDebugWindow() {
    showDebugWindow = !showDebugWindow;
}

Marmalade::GUI::Dialog Marmalade::GUI::WindowManager::RegisterDialog(std::shared_ptr<CustomDialog> customDlg, const DialogCallback& callback, bool reregister, ImGuiWindowFlags flags, ImVec2 minSize) {
    auto it = std::find_if(dialogs.begin(), dialogs.end(), [&](const Dialog& d) {
        return d.Name == customDlg->GetName();
    });

    if (it != dialogs.end()) {
        if (reregister) {
            dialogs.erase(it);
        } else {
            Dialog& foundDialog = *it;
            foundDialog.Callback = callback;
            return foundDialog;
        }
    }

    Dialog dialog{customDlg, customDlg->GetName(), flags, minSize, callback};
    dialogs.push_back(dialog);
    return dialog;
}

void Marmalade::GUI::WindowManager::ShowDialog(std::string name) {
    auto it = std::find_if(dialogs.begin(), dialogs.end(), [&](const Dialog& d) {
        return d.Name == name;
    });

    if (it != dialogs.end()) {
        Dialog& foundDialog = *it;
        foundDialog.CustomDlg->visible = true;
    }
}

IGFD::FileDialogConfig Marmalade::GUI::WindowManager::PrepareFileDialogConfig(const std::string& path, ImGuiFileDialogFlags flags) {
    IGFD::FileDialogConfig config;
    if (path.empty()) {
        config.path = GET_APP.GetCurrentProject() == nullptr ? "" : GET_APP.GetCurrentProject()->basePath.string();
    } else {
        config.path = path;
    }
    config.flags = flags;

    return config;
}

Marmalade::GUI::Dialog Marmalade::GUI::WindowManager::RegisterFileDialog(std::string name, const DialogCallback& callback, ImGuiWindowFlags flags, ImVec2 minSize) {
    auto it = std::find_if(fileDialogs.begin(), fileDialogs.end(), [&](const Dialog& d) {
        return d.Name == name;
    });

    if (it != fileDialogs.end()) {
        Dialog& foundDialog = *it;
        foundDialog.Callback = callback;
        return foundDialog;
    }

    Dialog dialog{nullptr, name, flags, minSize, callback};
    fileDialogs.push_back(dialog);
    return dialog;
}
