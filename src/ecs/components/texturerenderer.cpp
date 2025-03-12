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

#include <ecs/components/texturerenderer.h>

#include "../../application/config.h"
#include "../../application/application.h"

#include <scene/entity.h>

#include <imgui.h>
#include <ImGuiFileDialog.h>

#include <spdlog/spdlog.h>

void Marmalade::ECS::TextureRenderer::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    GLuint textureID = entity->renderable.GetTexture();
    if (textureID == 0) {
        ImGui::Text("No texture available");
    } else {
        ImGui::Image(ImTextureID(textureID), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_BROWSER_FILE")) {
            auto projectItem = *(Marmalade::GUI::ProjectItem*) payload->Data;
            if (projectItem.Type != GUI::FileType_IMAGE) {
                spdlog::error("Texture must be an image");
            } else {
                entity->renderable.SetTexture(projectItem.Path);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::Button("Choose Texture")) {
        auto* project = Application::GetInstance().GetCurrentProject();
        if (project) {
            IGFD::FileDialogConfig config;
            config.path = (project->basePath / "assets").string();
            config.flags = ImGuiFileDialogFlags_Modal;

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

    // Image Settings

    const char* filterModes[] = { "Nearest", "Linear", "Mipmap Nearest", "Mipmap Linear" };
    int minFilterIdx = (entity->renderable.texSettings.minFilter == GL_NEAREST) ? 0 :
                       (entity->renderable.texSettings.minFilter == GL_LINEAR) ? 1 :
                       (entity->renderable.texSettings.minFilter == GL_NEAREST_MIPMAP_NEAREST) ? 2 : 3;

    if (ImGui::Combo("Min Filter", &minFilterIdx, filterModes, IM_ARRAYSIZE(filterModes))) {
        entity->renderable.texSettings.minFilter =
                (minFilterIdx == 0) ? GL_NEAREST :
                (minFilterIdx == 1) ? GL_LINEAR :
                (minFilterIdx == 2) ? GL_NEAREST_MIPMAP_NEAREST :
                                    GL_LINEAR_MIPMAP_LINEAR;

        entity->renderable.UpdateTextureSettings();
    }

    const char* magFilterModes[]= { "Point" , "Nearest", "Bilinear" };
    int magFilterIdx = (entity->renderable.texSettings.magFilter == GL_POINT) ? 0 :
                       (entity->renderable.texSettings.magFilter == GL_NEAREST) ? 1 :
                       (entity->renderable.texSettings.magFilter == GL_LINEAR) ? 2 : 3;

    if (ImGui::Combo("Mag Filter", &magFilterIdx, magFilterModes, IM_ARRAYSIZE(magFilterModes))) {
        entity->renderable.texSettings.magFilter =
                (magFilterIdx == 0) ? GL_POINT :
                (magFilterIdx == 1) ? GL_NEAREST :
                (magFilterIdx == 2) ? GL_LINEAR  : GL_LINEAR;

        entity->renderable.UpdateTextureSettings();
    }

}

void Marmalade::ECS::TextureRenderer::Apply(Entity* entity) {
}

void Marmalade::ECS::TextureRenderer::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::TextureRenderer::Serialize(const Entity* entity) {
    nlohmann::json j;
    j["path"] = entity->renderable.texSettings.filePath;
    j["minFilter"] = entity->renderable.texSettings.minFilter;
    j["magFilter"] = entity->renderable.texSettings.magFilter;

    return j;
}

void Marmalade::ECS::TextureRenderer::Deserialize(nlohmann::json json, Entity* entity) {
    entity->renderable.SetTexture(json["path"]);
    entity->renderable.texSettings.minFilter = json["minFilter"].get<int>();
    entity->renderable.texSettings.magFilter = json["magFilter"].get<int>();
}
