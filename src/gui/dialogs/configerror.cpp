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

#include "configerror.h"

#include "../../application/application.h"
#include "../../application/plugins/interfaceimpl.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::ConfigErrorDialog::Draw() {
    WINDOW_BEGIN_MODAL(ICON_CI_ERROR " Config Error", ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(ICON_CI_ERROR "Failed to load config file: %s", fileName.c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", errorMsg.c_str());

    if (recreateConfigCallback != nullptr) {
        if (ImGui::Button("Recreate Config (this may result in lost data)")) {
            recreateConfigCallback();
            visible = false;
        }
    }

    if (ImGui::Button("Quit Marmalade Engine")) {
        glfwSetWindowShouldClose(Application::GetInstance().getWindow(), true);
    }

    if (ImGui::Button("Cancel")) {
        visible = false;
    }

    WINDOW_END_MODAL()
}
