
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

#include <../../../include/ecs/components/animation/animator.h>

#include "../../../application/config/engineconfig.h"
#include "../../../application/util.h"
#include "../../../application/application.h"

#include <imgui.h>
#include <ImGuiFileDialog.h>

#include <IconsCodicons.h>

#ifdef _MSC_VER
Marmalade::ECS::AnimationPlayer::AnimationPlayer() {
    ANIMATION_PLAYER_CTOR_BODY
}
#endif

void Marmalade::ECS::AnimationPlayer::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (!driver) {
        if (ImGui::Button(ICON_CI_PLUS " Create Animation Driver")) {
            _isCreatingDriver = true;
            ImGui::OpenPopup("CreateDriver");
        } else {
            ImGui::Text("Using driver: %s", driverPath.filename().string().c_str());
            if (ImGui::Button(ICON_CI_FOLDER_OPENED " Load Driver")) {
                IGFD::FileDialogConfig config;
                config.path = GET_APP.GetCurrentProject()->basePath.string();
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("LoadDriver", "Load Driver", ".animdvr", config);
            }

            if (ImGuiFileDialog::Instance()->Display("LoadDriver")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    driverPath = ImGuiFileDialog::Instance()->GetFilePathName();

                    std::ifstream file(driverPath);
                    if (file.is_open()) {
                        nlohmann::json jsonData;
                        file >> jsonData;
                        file.close();

                        driver = std::make_unique<Marmalade::Animation::AnimationDriver>(driverPath, "");
                        driver->Deserialise(jsonData);
                    } else {
                        LOG_ERROR("Failed to open the driver file: {}", driverPath.string());
                    }
                }
                ImGuiFileDialog::Instance()->Close();
            }
        }
    }

    showCreatePopup();
}

void Marmalade::ECS::AnimationPlayer::Apply(Entity* entity) {
}

void Marmalade::ECS::AnimationPlayer::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::AnimationPlayer::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["driverPath"] = driverPath.string();

    return j;
}

void Marmalade::ECS::AnimationPlayer::Deserialize(nlohmann::json json, Entity* entity) {
    driverPath = json["driverPath"].get<std::string>();

    std::ifstream file(driverPath);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        file.close();

        driver = std::make_unique<Marmalade::Animation::AnimationDriver>(driverPath, "");
        driver->Deserialise(j);
    } else {
        LOG_ERROR("Failed to open the driver file: {}", driverPath.string());
    }
}

void Marmalade::ECS::AnimationPlayer::showCreatePopup() {
    if (ImGui::BeginPopupModal("CreateDriver", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char driverName[128] = "New Animation Driver";

        ImGui::InputText("Driver Name ", driverName, IM_ARRAYSIZE(driverName));

        if (ImGui::Button(ICON_CI_FOLDER_OPENED " Choose location")) {
            IGFD::FileDialogConfig config;
            config.path = GET_APP.GetCurrentProject()->basePath.string();
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDriverPath", "Save Animation Driver", ".animdvr", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseDriverPath")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                driverPath = ImGuiFileDialog::Instance()->GetFilePathName();
                LOG_INFO("Driver save path: {}", driverPath.string());
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::Button("Create")) {
            if (!driverPath.empty()) {
                driver = std::make_unique<Marmalade::Animation::AnimationDriver>(driverPath, driverName);
                driver->storedConfig.name = driver->name;
                driver->storedConfig.uuid = Marmalade::Util::GenerateUUIDv4();
                driver->SaveConfig();

                memset(driverName, 0, sizeof(driverName));
                _isCreatingDriver = false;
                ImGui::CloseCurrentPopup();
            } else {
                ImGui::TextColored(ImColor(255, 0, 0), "Location is invalid");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            _isCreatingDriver = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
