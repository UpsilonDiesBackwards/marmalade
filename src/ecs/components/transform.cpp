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
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool posChanged = false;
        posChanged |= ImGui::DragFloat(("##PosX" + std::to_string(entity->id)).c_str(), &transform->pos.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        posChanged |= ImGui::DragFloat(("##PosY" + std::to_string(entity->id)).c_str(), &transform->pos.y, 0.1f);
        ImGui::PopItemWidth();

        if (posChanged) {
            entity->setPosition(glm::vec2(transform->pos.x, transform->pos.y));
        }

        // Rotation
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Rotation");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat(("##Rot" + std::to_string(entity->id)).c_str(), &transform->rotation, 0, 360)) {
            entity->setRotation(transform->rotation);
        }
        ImGui::PopItemWidth();

        // Scale
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Scale");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool scaleChanged = false;
        scaleChanged |= ImGui::DragFloat(("##ScaX" + std::to_string(entity->id)).c_str(), &transform->scale.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        scaleChanged |= ImGui::DragFloat(("##ScaY" + std::to_string(entity->id)).c_str(), &transform->scale.y, 0.1f);
        ImGui::PopItemWidth();

        if (scaleChanged) {
            entity->setScale(glm::vec2(transform->scale.x, transform->scale.y));
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

    j["rot"] = rotation;

    j["scale"]["x"] = scale.x;
    j["scale"]["y"] = scale.y;

    return j;
}

void Marmalade::ECS::Transform::Deserialize(nlohmann::json json, Entity* entity) {
    pos.x = json["pos"]["x"].get<float>();
    pos.y = json["pos"]["y"].get<float>();

    rotation = json["rot"].get<float>();

    scale.x = json["scale"]["x"].get<float>();
    scale.y = json["scale"]["y"].get<float>();
}