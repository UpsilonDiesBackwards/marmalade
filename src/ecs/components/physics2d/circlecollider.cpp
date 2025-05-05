
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
#include "../../../gui/components/backgroundlabel.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

void Marmalade::ECS::CircleCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, DataCircle>) {
            if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

                // Radius
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Radius");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                Marmalade::GUI::Components::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);
                if (ImGui::SliderFloat(("##Radius" + std::to_string(entity->id)).c_str(), &colliderData.radius, 0, 360)) {
                    colliderData.radius = colliderData.radius;
                }
                ImGui::PopItemWidth();

                // Offset
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Offset");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                Marmalade::GUI::Components::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                bool offsetChanged = false;
                offsetChanged |= ImGui::DragFloat(("##OffsetX" + std::to_string(entity->id)).c_str(), &colliderData.offset.x, 0.1f);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                Marmalade::GUI::Components::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                offsetChanged |= ImGui::DragFloat(("##OffsetY" + std::to_string(entity->id)).c_str(), &colliderData.offset.y, 0.1f);
                ImGui::PopItemWidth();

                if (offsetChanged) {
                    colliderData.offset = (glm::vec2(colliderData.offset.x, colliderData.offset.y));
                }

                ImGui::EndTable();
            }
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
            j["offset"]["x"] = colliderData.offset.x;
            j["offset"]["y"] = colliderData.offset.y;
        }
    }, data);

    return j;
}

void Marmalade::ECS::CircleCollider::Deserialize(nlohmann::json json, Entity* entity) {
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, DataCircle>) {
            colliderData.radius = json["radius"].get<float>();
            colliderData.offset.x = json["offset"]["x"].get<float>();
            colliderData.offset.y = json["offset"]["y"].get<float>();
        }
    }, data);
}

void Marmalade::ECS::CircleCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    auto* circleA = GetCollisionData<DataCircle>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();

    glm::vec2 posA = self->getPosition() + circleA->offset;
    glm::vec2 posB = other->getPosition();

    if (auto* circleCollider = other->componentManager.GetComponentOfType<CircleCollider>()) {
        if (auto* circleB = circleCollider->GetCollisionData<DataCircle>()) {
            posB += circleB->offset;
            float radii = circleA->radius + circleB->radius;
            float distSq = glm::dot(posB - posA, posB - posA);

            if (distSq <= radii * radii) {
                if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                    glm::vec2 collisionNorm = glm::normalize(posB - posA);
                    glm::vec2 ptOnA_WorldSpace = posA + collisionNorm * circleA->radius;
                    glm::vec2 ptOnB_WorldSpace = posB - collisionNorm * circleB->radius;

                    rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
                }
            }
        }
    }

    if (IntersectsAABB(*otherCollider, posA, posB)) {
        if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
            glm::vec2 closestPoint = glm::clamp(posA - posB - otherCollider->GetCollisionData<AABBDataBox>()->offset,
                                                -otherCollider->GetCollisionData<AABBDataBox>()->size * 0.5f,
                                                otherCollider->GetCollisionData<AABBDataBox>()->size * 0.5f) +
                                     posB + otherCollider->GetCollisionData<AABBDataBox>()->offset;

            glm::vec2 collisionNorm = glm::normalize(posA - closestPoint);
            glm::vec2 ptOnA_WorldSpace = posA - collisionNorm * circleA->radius;
            glm::vec2 ptOnB_WorldSpace = closestPoint;

            rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
        }
    }
}

bool Marmalade::ECS::CircleCollider::IntersectsAABB(const Marmalade::ECS::ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    if (const auto* aabb = std::get_if<AABBDataBox>(&other.data)) {
        glm::vec2 halfExtents = aabb->size * 0.5f;

        glm::vec2 closestPoint = glm::clamp(posA - posB - aabb->offset, -halfExtents, halfExtents) + posB  + aabb->offset;

        float distSquared = glm::dot(closestPoint - posA, closestPoint - posA);
        float radiusSquared = std::get<DataCircle>(data).radius * std::get<DataCircle>(data).radius;

        return distSquared < radiusSquared;
    }

    return false;
}

void Marmalade::ECS::CircleCollider::ShowBounds(const glm::vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<DataCircle>(&data)) {
        glm::vec2 centre = entityPosition + aabbData->offset + glm::vec2(aabbData->radius, aabbData->radius);
        float radius = aabbData->radius;

        ImVec2 screenCentre = EditorViews::WorldToScreenSpace(centre);
        float screenRadius = WorldRadiusToScreenScale(radius);

        ImGui::GetWindowDrawList()->AddCircle(
                screenCentre, screenRadius,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)),
                32, 2.0f
                );
    }
}

float Marmalade::ECS::CircleCollider::WorldRadiusToScreenScale(float radius) {
    ImVec2 screenStart = EditorViews::WorldToScreenSpace(glm::vec2(0.0f, 0.0f));
    ImVec2 screenEnd = EditorViews::WorldToScreenSpace(glm::vec2(radius, 0.0f));

    glm::vec2 glmScreenStart(screenStart.x, screenStart.y);
    glm::vec2 glmScreenEnd(screenEnd.x, screenEnd.y);

    return glm::distance(glmScreenStart, glmScreenEnd);
}
