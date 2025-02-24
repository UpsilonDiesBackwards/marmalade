
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

#include "ecs/components/physics/boxcollider.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void Marmalade::ECS::BoxCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    float rotation = entity->getRotation();

    if (rotation == 0.0f) {
        if (!std::holds_alternative<AABBData>(data)) {
            OBBData prevData = std::get<OBBData>(data);
            data = AABBData{prevData.size, prevData.offset};
        }
    } else if (!std::holds_alternative<OBBData>(data)) {
        AABBData prevData = std::get<AABBData>(data);
        data = OBBData{prevData.size, prevData.offset, rotation};
    }

    std::visit([&](auto &colliderData) {
        ImGui::DragFloat2("Size", glm::value_ptr(colliderData.size), 0.1f);
        ImGui::DragFloat2("Offset", glm::value_ptr(colliderData.offset), 0.1f);
    }, data);

    ImGui::Checkbox("Draw Bounds", &showingBounds);
}

void Marmalade::ECS::BoxCollider::Apply(Entity* entity) {
    for (auto other : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        if (other.get() == entity) { continue; }

        if (!other->componentManager.GetComponentOfType<BoxCollider>()) { return; }

        Intersects(entity, other.get());
    }
}

void Marmalade::ECS::BoxCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    glm::vec2 posA = self->getPosition();
    glm::vec2 posB = other->getPosition();

    auto* aabbA = GetCollisionData<AABBData>();
    auto* aabbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<AABBData>();

    auto* obbA = GetCollisionData<OBBData>();
    auto* obbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<OBBData>();

    if (aabbA && aabbB) {
        if (IntersectsAABB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            spdlog::info("Collision detected using AABB");
        }
    } else if (obbA && obbB) {
        spdlog::info("Collision detected using OBB");
    } else { spdlog::error("Invalid collision type pair!"); }
}

bool Marmalade::ECS::BoxCollider::IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    auto* aabbA = GetCollisionData<AABBData>();
    auto* aabbB = std::get_if<AABBData>(&other.data);

    if (!aabbA || !aabbB) return false;

    glm::vec2 minA = posA + aabbA->offset;
    glm::vec2 maxA = minA + aabbA->size;

    glm::vec2 minB = posB + aabbB->offset;
    glm::vec2 maxB = minB + aabbB->size;

    return (minA.x < maxB.x && maxA.x > minB.x &&
            minA.y < maxB.y && maxA.y > minB.y);
}

bool Marmalade::ECS::BoxCollider::IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    return false;
}

void Marmalade::ECS::BoxCollider::ShowBounds(const glm::vec2& entityPosition, Transform transform) {
    std::visit([&](auto& colliderData) {
        glm::vec2 pos = entityPosition + colliderData.offset;

        if constexpr (std::is_same_v<std::decay_t<decltype(colliderData)>, AABBData>) {
            glm::vec2 min = pos;
            glm::vec2 max = min + colliderData.size;

            ImVec2 screenMin = WorldToScreenSpace(min);
            ImVec2 screenMax = WorldToScreenSpace(max);

            ImGui::GetWindowDrawList()->AddRect(screenMin, screenMax,
                                                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 0.0f, 0.0f, 2.0f);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(colliderData)>, OBBData>) {
            float theta = glm::radians(transform.rotation);

            glm::vec2 corners[4] = {
                    {0, 0}, {colliderData.size.x, 0}, {colliderData.size.x, colliderData.size.y}, {0, colliderData.size.y}
            };

            glm::vec2 rotatedCorners[4];
            for (int i = 0; i < 4; ++i) {
                rotatedCorners[i] = pos + glm::vec2(
                                                  corners[i].x * cos(theta) - corners[i].y * sin(theta),
                                                  corners[i].x * sin(theta) + corners[i].y * cos(theta)
                                                 );
            }

            ImVec2 screenCorners[4];
            for (int i = 0; i < 4; ++i) {
                screenCorners[i] = WorldToScreenSpace(rotatedCorners[i]);
            }

            ImGui::GetWindowDrawList()->AddQuad(
                    screenCorners[0], screenCorners[1], screenCorners[2], screenCorners[3],
                    ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 2.0f
            );
        }
    }, data);
}
