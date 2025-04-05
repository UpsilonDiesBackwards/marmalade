
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
#include "ecs/components/physics2d/rigidbody.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

void Marmalade::ECS::CircleCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, DataCircle>) {
            ImGui::DragFloat("Radius",&colliderData.radius, 0.1f);
            ImGui::DragFloat2("Offset", colliderData.offset.ToPtr(), 0.1f);
        }
    }, data);

    ImGui::Checkbox("Draw Bounds", &showingBounds);
}

void Marmalade::ECS::CircleCollider::Apply(Entity* entity) {
    for (auto other : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        if (other.get() == entity) { continue; }

        auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();
        if (!otherCollider) { continue; }

        Intersects(entity, other.get());
    }
}

void Marmalade::ECS::CircleCollider::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::CircleCollider::Serialize(const Entity* entity) {
    nlohmann::json j;
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, DataCircle>) {
            j["radius"] = colliderData.radius;
            j["offset"]["x"] = colliderData.offset[0];
            j["offset"]["y"] = colliderData.offset[1];
        }
    }, data);

    return j;
}

void Marmalade::ECS::CircleCollider::Deserialize(nlohmann::json json, Entity* entity) {
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, DataCircle>) {
            colliderData.radius = json["radius"].get<float>();
            colliderData.offset[0] = json["offset"]["x"].get<float>();
            colliderData.offset[1] = json["offset"]["y"].get<float>();
        }
    }, data);
}

void Marmalade::ECS::CircleCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    auto* circleA = GetCollisionData<DataCircle>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();

    Marmalade::Mathematics::Vec2 posA = self->getPosition() + circleA->offset;
    Marmalade::Mathematics::Vec2 posB = other->getPosition();

    if (auto* circleCollider = other->componentManager.GetComponentOfType<CircleCollider>()) {
        if (auto* circleB = circleCollider->GetCollisionData<DataCircle>()) {
            posB += circleB->offset;
            float radii = circleA->radius + circleB->radius;
            float distSq = (posB - posA).Dot(posB - posA);

            if (distSq <= radii * radii) {
                if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                    Marmalade::Mathematics::Vec2 norm = posA - posB;
                    Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();
                    Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = posA + collisionNorm * circleA->radius;
                    Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = posB - collisionNorm * circleB->radius;

                    rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
                }
            }
        }
    }

    if (IntersectsAABB(*otherCollider, posA, posB)) {
        if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
            Marmalade::Mathematics::Vec2 offset = otherCollider->GetCollisionData<AABBDataBox>()->offset;
            Marmalade::Mathematics::Vec2 size = otherCollider->GetCollisionData<AABBDataBox>()->size;

            Marmalade::Mathematics::Vec2 closestPoint = (posA - posB - offset).Clamp(-size * 0.5f, size * 0.5f) + posB + offset;

            Marmalade::Mathematics::Vec2 norm = posA - closestPoint;
            Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();
            Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = posA - collisionNorm * circleA->radius;
            Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = closestPoint;

            rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
        }
    }
}

void Marmalade::ECS::CircleCollider::ShowBounds(const Marmalade::Mathematics::Vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<DataCircle>(&data)) {
        Marmalade::Mathematics::Vec2 centre = entityPosition + aabbData->offset + Marmalade::Mathematics::Vec2(aabbData->radius, aabbData->radius);
        float radius = aabbData->radius;

        ImVec2 screenCentre = EditorViews::WorldToScreenSpace(Marmalade::Mathematics::Vec2(centre[0], centre[1]));
        float screenRadius = WorldRadiusToScreenScale(radius);

        ImGui::GetWindowDrawList()->AddCircle(
                screenCentre, screenRadius,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)),
                32, 2.0f
                );
    }
}

bool Marmalade::ECS::CircleCollider::IntersectsAABB(const Marmalade::ECS::ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) {
    if (const auto* aabb = std::get_if<AABBDataBox>(&other.data)) {
        Marmalade::Mathematics::Vec2 halfExtents = aabb->size * 0.5f;

        Marmalade::Mathematics::Vec2 closestPoint = (posA - posB - aabb->offset).Clamp(-halfExtents, halfExtents) + posB + aabb->offset;

        float distSquared = (closestPoint - posA).Dot(closestPoint - posA);
        float radiusSquared = std::get<DataCircle>(data).radius * std::get<DataCircle>(data).radius;

        return distSquared < radiusSquared;
    }

    return false;
}

float Marmalade::ECS::CircleCollider::WorldRadiusToScreenScale(float radius) {
    ImVec2 screenStart = EditorViews::WorldToScreenSpace(Marmalade::Mathematics::Vec2(0.0f, 0.0f));
    ImVec2 screenEnd = EditorViews::WorldToScreenSpace(Marmalade::Mathematics::Vec2(radius, 0.0f));

    Marmalade::Mathematics::Vec2 glmScreenStart(screenStart[0], screenStart[1]);
    Marmalade::Mathematics::Vec2 glmScreenEnd(screenEnd[0], screenEnd[1]);

    return glmScreenStart.Distance(glmScreenStart, glmScreenEnd);
}
