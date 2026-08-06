// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "../include/ecs/components/physics/2d/boxcollider2d.h"

#include "ecs/components/physics/2d/rigidbody2d.h"
#include "../../../../scene/entity.h"

#include "../../../../physics/2d/collider2d.h"
#include "../../../../gui/windows/stylemanager.h"
#include "../../../../gui/components/backgroundlabel.h"

#ifdef _MSC_VER
Marmalade::ECS::BoxCollider2D::BoxCollider2D() {
    BOXCOLLIDER2D_CTOR_BODY
}
#endif

void Marmalade::ECS::BoxCollider2D::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        // Size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Scale");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool scChanged = false;
        scChanged |= ImGui::DragFloat(("##ScalX" + std::to_string(entity->id)).c_str(), &size.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        scChanged |= ImGui::DragFloat(("##ScalT" + std::to_string(entity->id)).c_str(), &size.y, 0.1f);
        ImGui::PopItemWidth();

        // Offset
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Offset");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool offChanged = false;
        offChanged |= ImGui::DragFloat(("##OffX" + std::to_string(entity->id)).c_str(), &offset.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        offChanged |= ImGui::DragFloat(("##OffY" + std::to_string(entity->id)).c_str(), &offset.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void Marmalade::ECS::BoxCollider2D::Apply(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        if (auto shape = std::dynamic_pointer_cast<Physics::BoxCollider2D>(rb->body.collider.shape)) {
            shape->size = this->size;
            shape->offset = this->offset;
        }
    }
}

void Marmalade::ECS::BoxCollider2D::Setup(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        rb->body.collider.type = Physics::ColliderType2D::Box;

        auto shape = std::make_shared<Physics::BoxCollider2D>();
        shape->size = this->size;
        shape->offset = this->offset;

        rb->body.collider.shape = shape;
    }
}

nlohmann::json Marmalade::ECS::BoxCollider2D::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["size"]["x"] = size.x;
    j["size"]["y"] = size.y;

    j["offset"]["x"] = offset.x;
    j["offset"]["y"] = offset.y;

    return j;
}

void Marmalade::ECS::BoxCollider2D::Deserialize(nlohmann::json json, Entity* entity) {
    size.x = json["size"]["x"].get<float>();
    size.y = json["size"]["y"].get<float>();

    offset.x = json["offset"]["x"].get<float>();
    offset.y = json["offset"]["y"].get<float>();
}