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

#include "../include/ecs/components/physics/2d/circlecollider2d.h"

#include "ecs/components/physics/2d/rigidbody2d.h"
#include "scene/entity.h"
#include "../../../../gui/components/backgroundlabel.h"
#include "../../../../gui/windows/stylemanager.h"

#ifdef _MSC_VER
Marmalade::ECS::CircleCollider2D::CircleCollider2D() {
    CIRCLECOLLIDER2D_CTOR_BODY
}
#endif

void Marmalade::ECS::CircleCollider2D::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        // Radius
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Radius");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::SliderFloat(("##Radius" + std::to_string(entity->id)).c_str(), &radius, 0, 360);
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
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        offChanged |= ImGui::DragFloat(("##OffY" + std::to_string(entity->id)).c_str(), &offset.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void Marmalade::ECS::CircleCollider2D::Apply(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        if (auto shape = std::dynamic_pointer_cast<Physics::CircleCollider2D>(rb->body.collider.shape)) {
            shape->radius = this->radius;
            shape->offset = this->offset;
        }
    }
}

void Marmalade::ECS::CircleCollider2D::Setup(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        rb->body.collider.type = Physics::ColliderType2D::Circle;

        auto shape = std::make_shared<Physics::CircleCollider2D>();

        shape->offset = this->offset;
        shape->radius = this->radius;

        rb->body.collider.shape = shape;
    }
}

nlohmann::json Marmalade::ECS::CircleCollider2D::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["radius"] = radius;

    j["offset"]["x"] = offset.x;
    j["offset"]["y"] = offset.y;

    return j;
}

void Marmalade::ECS::CircleCollider2D::Deserialize(nlohmann::json json, Entity* entity) {
    radius = json["radius"].get<float>();

    offset.x = json["offset"]["x"].get<float>();
    offset.y = json["offset"]["y"].get<float>();
}
