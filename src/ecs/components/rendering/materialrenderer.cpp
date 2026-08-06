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

#include <../../../include/ecs/components/rendering/materialrenderer.h>

#include "../../../application/config/config.h"
#include "../../../application/application.h"
#include "../../../application/logger.h"
#include "../../../gui/components/backgroundlabel.h"

#include <IconsCodicons.h>

#include <../../../scene/entity.h>

#include <imgui.h>
#include <ImGuiFileDialog.h>

void Marmalade::ECS::MaterialRenderer::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    Marmalade::Material::Material* m = entity->renderable.material.get();

    if (!m) {
        ImGui::Text("No material assigned.");

        if (ImGui::Button(ICON_CI_FOLDER_OPENED " Load Material")) {
            IGFD::FileDialogConfig config;
            config.path = GET_APP.GetCurrentProject()->basePath.string();
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("LoadMaterial", "Select Material", ".mmlmat", config);
        }

        if (ImGuiFileDialog::Instance()->Display("LoadMaterial")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                materialPath = ImGuiFileDialog::Instance()->GetFilePathName();

                std::ifstream file(materialPath);
                if (file.is_open()) {
                    nlohmann::json jsonData;
                    file >> jsonData;
                    file.close();

                    m = new Marmalade::Material::Material(materialPath, "", "");
                    m->Deserialise(jsonData);

                    entity->renderable.material = static_cast<const std::shared_ptr<Material::Material>>(m);
                } else {
                    LOG_ERROR("Failed to open the driver file: {}", m->filePath.string());
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    auto displaySlot = [](const char* label, Marmalade::Material::Texture& texture) {
        ImGui::SeparatorText(label);

        if (texture.id != 0) {
            ImGui::Image(ImTextureID(texture.id), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
        } else {
            ImGui::Text("No texture loaded");
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_BROWSER_FILE")) {
                auto projectItem = *(Marmalade::GUI::ProjectItem*)payload->Data;
                if (projectItem.Type != GUI::FileType_IMAGE) {
                    LOG_ERROR("Texture must be an image");
                } else {
                    texture.filePath = projectItem.Path;
                    texture.id = Texture::LoadTexture(texture.filePath, texture.settings);
                }
            }
            ImGui::EndDragDropTarget();
        }

        float buttonWidth = ImGui::CalcTextSize("Choose Texture##").x + ImGui::GetStyle().FramePadding.x * 2;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);

        if (ImGui::Button(std::string("Choose Texture##" + std::string(label)).c_str())) {
            auto* project = Application::GetInstance().GetCurrentProject();
            if (project) {
                IGFD::FileDialogConfig config;
                config.path = (project->basePath / "assets").string();
                config.flags = ImGuiFileDialogFlags_Modal;

                ImGuiFileDialog::Instance()->OpenDialog(std::string("SelectTexture_" + std::string(label)).c_str(), "Select image file", ".png,.jpg,.jpeg", config);
            } else {
                ImGui::Text("No project loaded!");
            }
        }

        if (ImGuiFileDialog::Instance()->Display(std::string("SelectTexture_" + std::string(label)).c_str())) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                texture.filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                texture.id = Texture::LoadTexture(texture.filePath, texture.settings);
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Indent();

        if (ImGui::CollapsingHeader((std::string("Settings##") + label).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

            // Filter Settings
            const char* filterModes[] = {"Nearest", "Linear", "Mipmap Nearest", "Mipmap Linear"};
            int minFilterIdx = (texture.settings.minFilter == GL_NEAREST) ? 0 : (texture.settings.minFilter == GL_LINEAR)           ? 1
                                                                        : (texture.settings.minFilter == GL_NEAREST_MIPMAP_NEAREST) ? 2
                                                                                                                                    : 3;

            if (ImGui::Combo((std::string("Min Filter##") + label).c_str(), &minFilterIdx, filterModes, IM_ARRAYSIZE(filterModes))) {
                texture.settings.minFilter =
                        (minFilterIdx == 0) ? GL_NEAREST : (minFilterIdx == 1) ? GL_LINEAR
                                                   : (minFilterIdx == 2)       ? GL_NEAREST_MIPMAP_NEAREST
                                                                               : GL_LINEAR_MIPMAP_LINEAR;

                glBindTexture(GL_TEXTURE_2D, texture.id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.settings.minFilter);
            }

            int magFilterIdx = (texture.settings.magFilter == GL_NEAREST) ? 0 : (texture.settings.magFilter == GL_LINEAR) ? 1
                                                                                                                          : 2;

            if (ImGui::Combo((std::string("Mag Filter##") + label).c_str(), &magFilterIdx, filterModes, IM_ARRAYSIZE(filterModes))) {
                texture.settings.magFilter =
                        (magFilterIdx == 0) ? GL_NEAREST : (magFilterIdx == 1) ? GL_LINEAR
                                                                               : GL_LINEAR;

                glBindTexture(GL_TEXTURE_2D, texture.id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.settings.magFilter);
            }

            if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

                // UV Scale
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("UV Scale");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                bool uvScaleChanged = false;
                uvScaleChanged |= ImGui::DragFloat(("##UVScaleX" + std::to_string(texture.id)).c_str(), &texture.settings.uvScaleX, 0.1f);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                uvScaleChanged |= ImGui::DragFloat(("##UVScaleY" + std::to_string(texture.id)).c_str(), &texture.settings.uvScaleY, 0.1f);
                ImGui::PopItemWidth();

                if (uvScaleChanged) {
                    texture.settings.uvScaleX = texture.settings.uvScaleX;
                    texture.settings.uvScaleY = texture.settings.uvScaleY;
                }

                ImGui::EndTable();
            }
        }

        ImGui::Unindent();
    };

    m = entity->renderable.material.get();

    if (!m) return;

    if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_DefaultOpen)) {
        displaySlot("Albedo", m->albedo);
    }
    if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_DefaultOpen)) {
        displaySlot("Normal", m->normal);
    }
    if (ImGui::CollapsingHeader("Specular", ImGuiTreeNodeFlags_DefaultOpen)) {
        displaySlot("Specular", m->specular);
    }
    if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_DefaultOpen)) {
        displaySlot("Roughness", m->roughness);
    }

    m->SaveConfig();
}

void Marmalade::ECS::MaterialRenderer::Apply(Entity* entity) {
}

void Marmalade::ECS::MaterialRenderer::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::MaterialRenderer::Serialize(const Entity* entity) {
    nlohmann::json j;

    if (entity->renderable.material) {
        j["materialPath"] = entity->renderable.material->filePath;

        const auto& material = entity->renderable.material;
        j["textures"]["albedo"] = SerializeTexture(material->albedo);
        j["textures"]["normal"] = SerializeTexture(material->normal);
        j["textures"]["specular"] = SerializeTexture(material->specular);
        j["textures"]["roughness"] = SerializeTexture(material->roughness);
    }

    return j;
}


void Marmalade::ECS::MaterialRenderer::Deserialize(nlohmann::json json, Entity* entity) {
    if (json.contains("materialPath")) {
        std::string materialPath = json["materialPath"].get<std::string>();

        auto material = std::make_shared<Marmalade::Material::Material>(materialPath, "", "");
        material->filePath = std::filesystem::path(materialPath);

        if (json.contains("textures")) {
            const auto& textures = json["textures"];

            if (textures.contains("albedo")) {
                DeserializeTexture(textures["albedo"], material->albedo);
                material->SetTexture(Marmalade::Material::TextureMode::Albedo, material->albedo.filePath);
            }

            if (textures.contains("normal")) {
                DeserializeTexture(textures["normal"], material->normal);
                material->SetTexture(Marmalade::Material::TextureMode::Normal, material->normal.filePath);
            }

            if (textures.contains("specular")) {
                DeserializeTexture(textures["specular"], material->specular);
                material->SetTexture(Marmalade::Material::TextureMode::Specular, material->specular.filePath);
            }

            if (textures.contains("roughness")) {
                DeserializeTexture(textures["roughness"], material->roughness);
                material->SetTexture(Marmalade::Material::TextureMode::Roughness, material->roughness.filePath);
            }
        }
        entity->renderable.material = material;
    }
}


nlohmann::json Marmalade::ECS::MaterialRenderer::SerializeTexture(const Marmalade::Material::Texture& texture) {
    nlohmann::json tex;

    tex["filePath"] = texture.filePath;
    tex["settings"]["minFilter"] = texture.settings.minFilter;
    tex["settings"]["magFilter"] = texture.settings.magFilter;
    tex["settings"]["wrapS"] = texture.settings.wrapS;
    tex["settings"]["wrapT"] = texture.settings.wrapT;
    tex["settings"]["uvScale"]["x"] = texture.settings.uvScaleX;
    tex["settings"]["uvScale"]["y"] = texture.settings.uvScaleY;

    return tex;
}

void Marmalade::ECS::MaterialRenderer::DeserializeTexture(const nlohmann::json& textureJson, Marmalade::Material::Texture& texture) {
    texture.filePath = textureJson["filePath"].get<std::string>();
    texture.settings.minFilter = textureJson["settings"]["minFilter"].get<GLuint>();
    texture.settings.magFilter = textureJson["settings"]["magFilter"].get<GLuint>();
    texture.settings.wrapS = textureJson["settings"]["wrapS"].get<GLuint>();
    texture.settings.wrapT = textureJson["settings"]["wrapT"].get<GLuint>();
    texture.settings.uvScaleX = textureJson["settings"]["uvScale"]["x"].get<float>();
    texture.settings.uvScaleY = textureJson["settings"]["uvScale"]["y"].get<float>();
}
