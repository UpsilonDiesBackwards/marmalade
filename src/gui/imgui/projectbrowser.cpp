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

#include "projectbrowser.h"

#include "../../application/application.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::ProjectBrowser::Draw() {
    ImGui::SetNextWindowPos(ImVec2(256, 128), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(720, 380), ImGuiCond_FirstUseEver);

    ImGui::Begin(ICON_CI_ZOOM_IN " Project Browser", &visible);

    auto* project = Application::GetInstance().GetCurrentProject();

    if (!project) {
        ImGui::Text("Open a project to view its assets");
        ImGui::End();
        return;
    }

    std::filesystem::path rootAssetFolder = std::filesystem::path(project->filePath) / "assets";

    currentPath = rootAssetFolder;

    if (currentPath != rootAssetFolder) {
        if (ImGui::Button("..")) {
            currentPath = currentPath.parent_path();
        }
    }

    for (const auto& item : std::filesystem::directory_iterator(currentPath)) {
        const std::filesystem::path path = item.path();
        bool isDirectory = item.is_directory();

        ImGui::BeginGroup();

        bool selected = ImGui::Selectable("##", false, ImGuiSelectableFlags_DontClosePopups);

        if (selected) {
            if (isDirectory) {
                currentPath = path;
            } else {
                // other stuff
            }
        }

        ImTextureID textureId;
        ImGui::SameLine();
        ImGui::Image(textureId, ImVec2(128, 128));
        ImGui::Text("%s", path.filename().string().c_str());

        ImGui::EndGroup();
    }

    ImGui::End();
}