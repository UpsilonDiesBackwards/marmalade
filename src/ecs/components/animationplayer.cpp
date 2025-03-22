
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

#include <ecs/components/animationplayer.h>

#include "../../application/config/engineconfig.h"

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

    if (!animation) {
        ImGui::TextColored(ImColor(255, 0, 0), "There is no animation sequence selected");


        if (ImGui::Button(ICON_CI_PLUS " New Sequence")) {
            _isCreatingSequence = true;
        }

        if (ImGui::Button(ICON_CI_FOLDER_OPENED " Select Sequence")) {
            IGFD::FileDialogConfig config;
            config.path = EngineConfig::GetStoredConfig().defaultProjectPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseSequence", "Choose Project File", ".animseq", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseSequence")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
             auto dir = ImGuiFileDialog::Instance()->GetCurrentPath();
             auto fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                std::filesystem::path projectPath = std::filesystem::path(dir) / fileName;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (_isCreatingSequence) {
            ImGui::OpenPopup("CreateSequence");
        }

        showCreatePopup();
        return;
    }

    ImGui::Text("Playing: %s", animation->name.c_str());

    if (ImGui::Button("Play Animation")) {
        animation->Play();
    }

    if (ImGui::Button("Stop Animation")) {
        animation->Stop();
    }
}

void Marmalade::ECS::AnimationPlayer::Apply(Entity* entity) {
}

void Marmalade::ECS::AnimationPlayer::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::AnimationPlayer::Serialize(const Entity* entity) {
    return nlohmann::json();
}

void Marmalade::ECS::AnimationPlayer::Deserialize(nlohmann::json json, Entity* entity) {
}


void Marmalade::ECS::AnimationPlayer::showCreatePopup() {
    if (ImGui::BeginPopupModal("CreateSequence", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char newSequenceName[128] = "";
        static std::filesystem::path newSequencePath;

        ImGui::InputText("Name", newSequenceName, IM_ARRAYSIZE(newSequenceName));

        ImGui::SameLine();

        if (ImGui::Button(ICON_CI_FOLDER_OPENED " Select Path")) {
            IGFD::FileDialogConfig config;
            config.path = EngineConfig::GetStoredConfig().defaultProjectPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("NewSequence", "Choose Location", nullptr, config);
        }

        if (ImGuiFileDialog::Instance()->Display("NewSequence")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                auto dir = ImGuiFileDialog::Instance()->GetCurrentPath();
                auto fileName = std::string(newSequenceName) + ".animseq";
                newSequencePath = std::filesystem::path(dir) / fileName;
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::Button("Create")) {
            auto newSequence = std::make_unique<Animation::AnimationSequence>(newSequencePath, newSequenceName);
            animation = std::move(newSequence);

            animation->SaveConfig();

            LOG_DEBUG("Created new animation: {}", animation->name);

            memset(newSequenceName, 0, sizeof(newSequenceName));
            _isCreatingSequence = false;
            ImGui::CloseCurrentPopup();
        }

        if (strlen(newSequenceName) == 0) {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Invalid name! Name cannot be empty.");
        }

        ImGui::EndPopup();
    }
}
