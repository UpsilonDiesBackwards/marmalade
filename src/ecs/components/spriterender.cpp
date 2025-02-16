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

#include <ecs/components/spriterender.h>

#include "../../application/config.h"
#include "../../application/application.h"

#include <scene/entity.h>

#include <imgui.h>
#include <ImGuiFileDialog.h>

void Marmalade::ECS::SpriteRender::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    GLuint textureID = entity->renderable.GetTexture();
    if (textureID == 0) {
        ImGui::Text("No texture available");
    } else {
        ImGui::Image(ImTextureID(textureID), ImVec2(128, 128));
    }

    if (ImGui::Button("Choose Texture")) {
        auto* project = Application::GetInstance().GetCurrentProject();
        if (project) {
            IGFD::FileDialogConfig config;
            config.path = project->filePath + "/assets";

            ImGuiFileDialog::Instance()->OpenDialog("SelectTexture", "Select image file", ".png,.jpg,.jpeg", config);
        } else {
            ImGui::Text("No project loaded!");
        }
    }

    if (ImGuiFileDialog::Instance()->Display("SelectTexture")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string texturePath = ImGuiFileDialog::Instance()->GetFilePathName();
            entity->renderable.SetTexture(texturePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Marmalade::ECS::SpriteRender::Apply(Entity* entity) {

}