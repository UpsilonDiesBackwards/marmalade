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

#include <ecs/components/transform.h>

#include "../../application/application.h"
#include "../../gui/components/backgroundlabel.h"
#include <scene/entity.h>

#include <imgui.h>
#include <glm/gtc/quaternion.hpp>

void Marmalade::ECS::Transform::Display(Entity* entity) {
    auto transform = entity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        // Translation
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Translation");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool posChanged = false;
        posChanged |= ImGui::DragFloat(("##PosX" + std::to_string(entity->id)).c_str(), &transform->pos.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        posChanged |= ImGui::DragFloat(("##PosY" + std::to_string(entity->id)).c_str(), &transform->pos.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Z ", COL_CATPPUCCIN_UI_BLUE);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        posChanged |= ImGui::DragFloat(("##PosZ" + std::to_string(entity->id)).c_str(), &transform->pos.z, 0.1f);
        ImGui::PopItemWidth();

        if (posChanged) {
            entity->setPosition(transform->pos);
        }

        // Rotation
        glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(transform->rotation));
        bool rotChanged = false;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Rotation");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);

        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        rotChanged |= ImGui::DragFloat(("##RotX" + std::to_string(entity->id)).c_str(), &eulerDegrees.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        rotChanged |= ImGui::DragFloat(("##RotY" + std::to_string(entity->id)).c_str(), &eulerDegrees.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Z ", COL_CATPPUCCIN_UI_BLUE);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        rotChanged |= ImGui::DragFloat(("##RotZ" + std::to_string(entity->id)).c_str(), &eulerDegrees.z, 0.1f);
        ImGui::PopItemWidth();

        if (rotChanged) {
            // Convert back to quaternion
            entity->setRotation(eulerDegrees);
        }


        // Scale
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Scale");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool scaleChanged = false;
        scaleChanged |= ImGui::DragFloat(("##ScaX" + std::to_string(entity->id)).c_str(), &transform->scale.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        scaleChanged |= ImGui::DragFloat(("##ScaY" + std::to_string(entity->id)).c_str(), &transform->scale.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Z ", COL_CATPPUCCIN_UI_BLUE);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        scaleChanged |= ImGui::DragFloat(("##ScaZ" + std::to_string(entity->id)).c_str(), &transform->scale.z, 0.1f);
        ImGui::PopItemWidth();

        if (scaleChanged) {
            entity->setScale(transform->scale);
        }

        ImGui::EndTable();
    }
}

void Marmalade::ECS::Transform::Apply(Entity* entity) {

}

void Marmalade::ECS::Transform::Setup(Entity* entity) {
    entity->UpdateModelMatrix();
}

nlohmann::json Marmalade::ECS::Transform::Serialize(const Entity* entity) {
    nlohmann::json j;
    j["pos"]["x"] = pos.x;
    j["pos"]["y"] = pos.y;
    j["pos"]["z"] = pos.z;

    j["rot"]["x"] = rotation.x;
    j["rot"]["y"] = rotation.y;
    j["rot"]["z"] = rotation.z;
    j["rot"]["w"] = rotation.w;

    j["scale"]["x"] = scale.x;
    j["scale"]["y"] = scale.y;
    j["scale"]["z"] = scale.z;

    return j;
}

void Marmalade::ECS::Transform::Deserialize(nlohmann::json json, Entity* entity) {
    pos.x = json["pos"]["x"].get<float>();
    pos.y = json["pos"]["y"].get<float>();
    pos.z = json["pos"]["z"].get<float>();

    rotation.x = json["rot"]["x"].get<float>();
    rotation.y = json["rot"]["y"].get<float>();
    rotation.z = json["rot"]["z"].get<float>();
    rotation.w = json["rot"]["w"].get<float>();

    scale.x = json["scale"]["x"].get<float>();
    scale.y = json["scale"]["y"].get<float>();
    scale.z = json["scale"]["z"].get<float>();
}