
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

#include "autosave.h"

#include "../../application/application.h"

#include <IconsCodicons.h>

#include <imgui.h>

void Marmalade::GUI::AutoSave::Draw() {
    auto now = std::chrono::steady_clock::now();
    if (now - GET_APP.autoSave.toastStartTime > GET_APP.autoSave.toastDuration) {
        visible = false;
        return;
    }

    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

    ImVec2 modalSize = ImVec2(235, 32);

    ImVec2 pos = ImVec2(viewportPos.x + viewportSize.x - modalSize.x - 10,
                        viewportPos.y + viewportSize.y - modalSize.y - 10);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(modalSize);

    if (ImGui::Begin(ICON_CI_SAVE " Autosave", &visible,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove)) {

        ImGui::Text("Autosaved: %s", GET_APP.autoSave.lastSaveTimeStamp.c_str());

        ImGui::End();
    }
}
