
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

    if (rotation == 0.0f || rotation == 360.f) {
        if (!std::holds_alternative<AABBData>(data)) {
            OBBData prevData = std::get<OBBData>(data);
            data = AABBData{prevData.size, prevData.offset};
        }
    } else if (!std::holds_alternative<OBBData>(data)) {
        AABBData prevData = std::get<AABBData>(data);

        glm::vec2 uX = glm::vec2(cos(rotation), sin(rotation));
        glm::vec2 uY = glm::vec2(-uX.y, uX.x);


        data = OBBData{prevData.size, prevData.offset, rotation,
                       prevData.offset, {uX, uY}, prevData.size * 0.5f};
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
        if (IntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            spdlog::info("Collision detected using OBB");
        }
    }
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
    auto* obbA = GetCollisionData<OBBData>();
    auto* obbB = std::get_if<OBBData>(&other.data);

    if (!obbA || !obbB) return false;

    float radiiA, radiiB;
    glm::mat2 R, AbsR;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            R[i][j] = glm::dot(obbA->u[i], obbB->u[j]);
        }
    }

//    spdlog::info("Entity A Pos: {}, {}", posA.x, posA.y);
//    spdlog::info("Entity B Pos: {}, {}", posB.x, posB.y);
//
//    spdlog::info("OBB A Center: {}, {}", obbA->c.x, obbA->c.y);
//    spdlog::info("OBB B Center: {}, {}", obbB->c.x, obbB->c.y);
//    spdlog::info("OBB A Half-Extents: {}, {}", obbA->e.x, obbA->e.y);
//    spdlog::info("OBB B Half-Extents: {}, {}", obbB->e.x, obbB->e.y);

//    spdlog::info("OBB A uX: s {}, {}", obbA->u[0].x, obbA->u[0].y);
//    spdlog::info("OBB A uY: {}, {}", obbA->u[1].x, obbA->u[1].y);
//    spdlog::info("OBB B uX: {}, {}", obbB->u[0].x, obbB->u[0].y);
//    spdlog::info("OBB B uY: {}, {}", obbB->u[1].x, obbB->u[1].y);

    // Compute translation vector
    glm::vec2 t = obbB->c - obbA->c;
    t = glm::vec2(glm::dot(t, obbA->u[0]), glm::dot(t, obbA->u[1]));

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            AbsR[i][j] = glm::max(glm::abs(R[i][j]), 1e-6f);
        }
    }

    // Test Axes L = A0/A1
    for (int i = 0; i < 2; ++i) {
        radiiA = obbA->e[i];
        radiiB = obbB->e[0] * AbsR[i][0] + obbB->e[1] * AbsR[i][1];
        if (glm::abs(t[i]) > radiiA + radiiB) return false;
    }

    // Test Axes L = B0/B1
    for (int i = 0; i < 2; ++i) {
        radiiA = obbA->e[0] * AbsR[0][i] + obbA->e[1] * AbsR[1][i];
        radiiB = obbB->e[i];
        if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i]) > radiiA + radiiB) return false;
    }

    return true; // No separating axis found, OBBs intersecting
}

void Marmalade::ECS::BoxCollider::ShowBounds(const glm::vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<AABBData>(&data)) {
        glm::vec2 min = entityPosition + aabbData->offset;
        glm::vec2 max = min + aabbData->size;

        ImVec2 screenMin = WorldToScreenSpace(min);
        ImVec2 screenMax = WorldToScreenSpace(max);

        ImGui::GetWindowDrawList()->AddRect(
                screenMin, screenMax,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 0.0f, 0.0f, 2.0f
        );
    } else if (auto* obbData = std::get_if<OBBData>(&data)) {
        obbData->c = entityPosition + obbData->offset;

        float theta = glm::radians(transform.rotation);

        glm::vec2 corners[4] = {
                {0, 0}, {obbData->size.x, 0}, {obbData->size.x, obbData->size.y}, {0, obbData->size.y}
        };

        glm::vec2 rotatedCorners[4];
        for (int i = 0; i < 4; ++i) {
            rotatedCorners[i] = obbData->c + glm::vec2(
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
}
