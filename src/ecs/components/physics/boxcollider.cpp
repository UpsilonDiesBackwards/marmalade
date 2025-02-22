
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

#include "../../../application/application.h"

#include <imgui.h>

void Marmalade::ECS::BoxCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    auto* aabb = GetCollisionData<AABBData>();
    if (aabb) {
        ImGui::DragFloat2("AABB Size", &aabb->size.x, 0.1f, 0.1f, 100.0f);
        ImGui::DragFloat2("AABB Offset", &aabb->offset.x, 0.1f, -10.0f, 10.0f);
    }

    auto* obb = GetCollisionData<OBBData>();
    if (obb) {
        ImGui::DragFloat2("OBB Size", &obb->size.x, 0.1f, 0.1f, 100.0f);
        ImGui::DragFloat2("OBB Offset", &obb->offset.x, 0.1f, -10.0f, 10.0f);
        ImGui::DragFloat("OBB Rotation", &obb->rotation, 0.1f, -180.0f, 180.0f);
    }

    ImGui::Checkbox("Draw Bounds", &_showBounds);
    if (_showBounds) {
        ShowBounds(entity);
    }
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

bool Marmalade::ECS::BoxCollider::IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB, float rotation) {
    return false;
}

void Marmalade::ECS::BoxCollider::ShowBounds(Entity* entity) {
    // TODO: Implement bounds rendering
}