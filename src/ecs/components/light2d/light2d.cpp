
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

#include "ecs/components/light2d/light2d.h"

#include "ecs/components/transform.h"
#include "scene/entity.h"
#include "../../../application/application.h"

#include <imgui.h>

#ifdef _MSC_VER
Marmalade::ECS::Light2D::Light2D() {
    LIGHT2D_CTOR_BODY
}
#endif

void Marmalade::ECS::Light2D::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::ColorPicker3("Color", reinterpret_cast<float*>(&color));
    ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 512.0f);
    ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 1024.0f);
    ImGui::DragFloat("Attenuation", &attenuation, 0.1f, 0.0f, 256.0f);
}

void Marmalade::ECS::Light2D::Apply(Entity* entity) {
    position = entity->getPosition();
}

void Marmalade::ECS::Light2D::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::Light2D::Serialize(const Entity* entity) {
    nlohmann::json j;
    j["pos"]["x"] = position.x;
    j["pos"]["y"] = position.y;

    j["color"]["r"] = color.r;
    j["color"]["g"] = color.g;
    j["color"]["b"] = color.b;

    j["intensity"] = intensity;

    j["radius"] = radius;
    j["attenuation"] = attenuation;

    return j;
}

void Marmalade::ECS::Light2D::Deserialize(nlohmann::json json, Entity* entity) {
    position.x = json["pos"]["x"].get<float>();
    position.y = json["pos"]["y"].get<float>();

    color.r = json["color"]["r"].get<float>();
    color.g = json["color"]["g"].get<float>();
    color.b = json["color"]["b"].get<float>();

    intensity = json["intensity"].get<float>();

    radius = json["radius"].get<float>();
    attenuation = json["attenuation"].get<float>();
}

glm::vec2 Marmalade::ECS::Light2D::GetPosition() {
    return position;
}

void Marmalade::ECS::Light2D::ShowBounds(Entity* entity) {
    glm::vec2 centre = entity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->pos;
    ImVec2 screenCentre = EditorViews::WorldToScreenSpace(centre);

    float screenRadius = WorldRadiusToScreenScale(radius);
    ImGui::GetWindowDrawList()->AddCircle(
            screenCentre, screenRadius,
            ImGui::GetColorU32(IM_COL32(255, 255, 0, 255)),
            32, 2.0f
    );

    float attenuationRadius = radius + attenuation;
    float screenAttenuation = WorldRadiusToScreenScale(attenuationRadius);
    ImGui::GetWindowDrawList()->AddCircle(
            screenCentre, screenAttenuation,
            ImGui::GetColorU32(IM_COL32(255, 0, 255, 128)),
            32, 1.0f
    );
}


float Marmalade::ECS::Light2D::WorldRadiusToScreenScale(float radius) {
    ImVec2 screenStart = EditorViews::WorldToScreenSpace(glm::vec2(0.0f, 0.0f));
    ImVec2 screenEnd = EditorViews::WorldToScreenSpace(glm::vec2(radius, 0.0f));

    glm::vec2 glmScreenStart(screenStart.x, screenStart.y);
    glm::vec2 glmScreenEnd(screenEnd.x, screenEnd.y);

    return glm::distance(glmScreenStart, glmScreenEnd);
}