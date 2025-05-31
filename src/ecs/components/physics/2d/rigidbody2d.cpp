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

#include "../include/ecs/components/physics/2d/rigidbody2d.h"

#include "../../../../application/application.h"

#include "../src/gui/components/backgroundlabel.h"

#ifdef _MSC_VER
Marmalade::ECS::Rigidbody2D::Rigidbody2D() {
    RIGIDBODY2D_CTOR_BODY
}
#endif

void Marmalade::ECS::Rigidbody2D::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::Checkbox("Static", &body.isStatic);

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        // Centre of Mass
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Offset");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool comChanged = false;
        comChanged |= ImGui::DragFloat(("##COMX" + std::to_string(entity->id)).c_str(), &body.centreOfMass.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        comChanged |= ImGui::DragFloat(("##COMY" + std::to_string(entity->id)).c_str(), &body.centreOfMass.y, 0.1f);
        ImGui::PopItemWidth();

        if (comChanged) { body.centreOfMass = (glm::vec2(body.centreOfMass.x, body.centreOfMass.y)); }

        // Velocity
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Velocity");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool velChanged = false;
        velChanged |= ImGui::DragFloat(("##VelX" + std::to_string(entity->id)).c_str(), &body.velocity.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        velChanged |= ImGui::DragFloat(("##VelY" + std::to_string(entity->id)).c_str(), &body.velocity.y, 0.1f);
        ImGui::PopItemWidth();

        if (velChanged) { body.velocity = (glm::vec2(body.velocity.x, body.velocity.y)); }

        // Gravity
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Gravity");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat(("##Grav" + std::to_string(entity->id)).c_str(), &body.gravity, 0, 360)) { body.gravity = body.gravity; }
        ImGui::PopItemWidth();

        // Mass
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Mass");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat(("##Mass" + std::to_string(entity->id)).c_str(), &body.mass, 0, 360)) { body.mass = body.mass; }
        ImGui::PopItemWidth();

        // TODO: Implement physics materials
        // Elasticity
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Elasticity");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat(("##Elast" + std::to_string(entity->id)).c_str(), &body.elasticity, 0, 360)) { body.elasticity = body.elasticity; }
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void Marmalade::ECS::Rigidbody2D::Apply(Entity* entity) {}

void Marmalade::ECS::Rigidbody2D::Setup(Entity* entity) {}

nlohmann::json Marmalade::ECS::Rigidbody2D::Serialize(const Entity* entity) {
    nlohmann::json j;

    j["isStatic"] = body.isStatic;
    j["velocity"]["x"] = body.velocity.x;
    j["velocity"]["y"] = body.velocity.y;

    j["gravity"] = body.gravity;

    j["mass"] = body.mass;
    j["inverseMass"] = body.inverseMass;

    j["elasticity"] = body.elasticity;

    return j;
}

void Marmalade::ECS::Rigidbody2D::Deserialize(nlohmann::json json, Entity* entity) {
    body.isStatic = json["isStatic"].get<bool>();

    body.velocity.y = json["velocity"]["y"].get<float>();
    body.velocity.x = json["velocity"]["x"].get<float>();

    body.gravity = json["gravity"].get<float>();

    body.mass = json["mass"].get<float>();
    body.inverseMass = json["inverseMass"].get<float>();

    body.elasticity = json["elasticity"].get<float>();
}