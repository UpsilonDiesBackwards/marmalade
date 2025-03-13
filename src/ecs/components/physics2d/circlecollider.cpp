
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

#include "ecs/components/physics2d/circlecollider.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

void Marmalade::ECS::CircleCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    float rotation = entity->getRotation();

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataCircle>) {
            ImGui::DragFloat("Radius",&colliderData.radius, 0.1f);
            ImGui::DragFloat2("Offset", glm::value_ptr(colliderData.offset), 0.1f);
        }
    }, data);

    ImGui::Checkbox("Draw Bounds", &showingBounds);
}

void Marmalade::ECS::CircleCollider::Apply(Entity* entity) {
}

void Marmalade::ECS::CircleCollider::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::CircleCollider::Serialize(const Entity* entity) {
    return nlohmann::json();
}

void Marmalade::ECS::CircleCollider::Deserialize(nlohmann::json json, Entity* entity) {
}

void Marmalade::ECS::CircleCollider::Intersects(Entity* self, Entity* other) {
}

bool Marmalade::ECS::CircleCollider::IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    return false;
}

bool Marmalade::ECS::CircleCollider::IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    return false;
}

glm::vec2 Marmalade::ECS::CircleCollider::CalculateOBBCentrePoint(const glm::vec2& entityPosition, const glm::vec2& offset) {
    return glm::vec2();
}

void Marmalade::ECS::CircleCollider::ShowBounds(const glm::vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<AABBDataCircle>(&data)) {
        glm::vec2 centre = entityPosition + aabbData->offset + glm::vec2(aabbData->radius, aabbData->radius);
        float radius = aabbData->radius;

        ImVec2 screenCentre = WorldToScreenSpace(centre);
        float screenRadius = WorldRadiusToScreenScale(radius);

        ImGui::GetWindowDrawList()->AddCircle(
                screenCentre, screenRadius,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)),
                32, 2.0f
                );
    }
}

float Marmalade::ECS::CircleCollider::WorldRadiusToScreenScale(float radius) {
    ImVec2 screenStart = WorldToScreenSpace(glm::vec2(0.0f, 0.0f));
    ImVec2 screenEnd = WorldToScreenSpace(glm::vec2(radius, 0.0f));

    glm::vec2 glmScreenStart(screenStart.x, screenStart.y);
    glm::vec2 glmScreenEnd(screenEnd.x, screenEnd.y);

    return glm::distance(glmScreenStart, glmScreenEnd);
}
