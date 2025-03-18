
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

    float rotation = entity->getRotation();

    if (rotation == 0.0f || rotation == 360.0f) {
        if (!std::holds_alternative<AABBDataCircle>(data)) {
            OBBDataCircle prevData = std::get<OBBDataCircle>(data);
            data = AABBDataCircle{prevData.radius, prevData.offset};
        }
    } else if (!std::holds_alternative<OBBDataCircle>(data)) {
        AABBDataCircle prevData = std::get<AABBDataCircle>(data);

        glm::vec2 uX = glm::vec2(cos(entity->getRotation()), -sin(entity->getRotation()));
        glm::vec2 uY = glm::vec2(sin(entity->getRotation()), cos(entity->getRotation()));

        data = OBBDataCircle{prevData.radius, prevData.offset, entity->getRotation(),
                             CalculateOBBCentrePoint(entity->getPosition(), prevData.offset),
                             {uX, uY}, glm::vec2(prevData.radius * 0.5f, prevData.radius * 0.5f)};
    }

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataCircle> || std::is_same_v<T, OBBDataCircle>) {
            ImGui::DragFloat("Radius",&colliderData.radius, 0.1f);
            ImGui::DragFloat2("Offset", glm::value_ptr(colliderData.offset), 0.1f);
        }
    }, data);

    ImGui::Checkbox("Draw Bounds", &showingBounds);
}

void Marmalade::ECS::CircleCollider::Apply(Entity* entity) {
    for (auto other : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        if (other.get() == entity) { continue; }

        if (!other->componentManager.GetComponentOfType<CircleCollider>()) { return; }

        if (auto* obbData = std::get_if<OBBDataCircle>(&data)) {
            OBBDataCircle prevData = std::get<OBBDataCircle>(data);

            glm::vec2 uX = glm::vec2(cos(entity->getRotation()), -sin(entity->getRotation()));
            glm::vec2 uY = glm::vec2(sin(entity->getRotation()), cos(entity->getRotation()));

            data = OBBDataCircle{prevData.radius, prevData.offset, entity->getRotation(),
                              CalculateOBBCentrePoint(entity->getPosition(), prevData.offset),
                              {uX, uY}, glm::vec2(prevData.radius * 0.5f, prevData.radius * 0.5f)};
        }
        Intersects(entity, other.get());
    }
}

void Marmalade::ECS::CircleCollider::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::CircleCollider::Serialize(const Entity* entity) {
    nlohmann::json j;
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, AABBDataCircle> || std::is_same_v<T, OBBDataCircle>) {
            j["radius"] = colliderData.radius;
            j["offset"]["x"] = colliderData.offset.x;
            j["offset"]["y"] = colliderData.offset.y;
        }
    }, data);

    return j;
}

void Marmalade::ECS::CircleCollider::Deserialize(nlohmann::json json, Entity* entity) {
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, AABBDataCircle> || std::is_same_v<T, OBBDataCircle>) {
            colliderData.radius = json["radius"].get<float>();
            colliderData.offset.x = json["offset"]["x"].get<float>();
            colliderData.offset.y = json["offset"]["y"].get<float>();
        }
    }, data);
}

void Marmalade::ECS::CircleCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    glm::vec2 posA = self->getPosition();
    glm::vec2 posB = other->getPosition();

    auto* aabbA = GetCollisionData<AABBDataCircle>();
    auto* aabbB = other->componentManager.GetComponentOfType<CircleCollider>()->GetCollisionData<AABBDataCircle>();

    auto* obbA = GetCollisionData<OBBDataCircle>();
    auto* obbB = other->componentManager.GetComponentOfType<CircleCollider>()->GetCollisionData<OBBDataCircle>();

    if (aabbA && aabbB) {
        if (IntersectsAABB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                glm::vec2 collisionNorm = glm::normalize(posA - posB);

                glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * aabbA->radius;
                glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * aabbB->radius;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});

                spdlog::info("COLLIDE!!");
            }
        }
    } else if (obbA && obbB) {
        if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
            glm::vec2 collisionNorm = glm::normalize(posA - posB);

            glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * obbA->radius;
            glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * obbA->radius;

            rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
        }
    }
}

bool Marmalade::ECS::CircleCollider::IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    spdlog::info("UwU");

    auto* aabbA = GetCollisionData<AABBDataCircle>();
    auto* aabbB = std::get_if<AABBDataCircle>(&other.data);

    if (!aabbA || !aabbB) return false;

    float d = (aabbA->radius / 2) - (aabbB->radius / 2);

    float dist = glm::dot(d, d);

    float radiusSum = aabbA->radius + aabbB->radius;

    return dist <= radiusSum * radiusSum;
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
