
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

#include "ecs/components/physics2d/boxcollider.h"

#include "ecs/components/physics2d/rigidbody.h"
#include "../../../gui/components/backgroundlabel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void Marmalade::ECS::BoxCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    float rotation = entity->getRotation();

    if (rotation == 0.0f || rotation == 360.f) {
        if (!std::holds_alternative<AABBDataBox>(data)) {
            OBBDataBox prevData = std::get<OBBDataBox>(data);
            data = AABBDataBox{prevData.size, prevData.offset};
        }
    } else if (!std::holds_alternative<OBBDataBox>(data)) {
        AABBDataBox prevData = std::get<AABBDataBox>(data);

        glm::vec2 uX = glm::vec2(cos(rotation), sin(rotation));
        glm::vec2 uY = glm::vec2(-uX.y, uX.x);

        data = OBBDataBox{prevData.size, prevData.offset, rotation,
                          prevData.offset, {uX, uY}, prevData.size * 0.5f};
    }

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

                // Size
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Size");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                bool sizeChanged = false;
                sizeChanged |= ImGui::DragFloat(("##SizeX" + std::to_string(entity->id)).c_str(), &colliderData.size.x, 0.1f);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                sizeChanged |= ImGui::DragFloat(("##SizeY" + std::to_string(entity->id)).c_str(), &colliderData.size.y, 0.1f);
                ImGui::PopItemWidth();

                if (sizeChanged) {
                    colliderData.size = (glm::vec2(colliderData.size.x, colliderData.size.y));
                }

                // Offset
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Offset");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                bool offsetChanged = false;
                offsetChanged |= ImGui::DragFloat(("##OffsetX" + std::to_string(entity->id)).c_str(), &colliderData.offset.x, 0.1f);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
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

void Marmalade::ECS::BoxCollider::Apply(Entity* entity) {
    for (auto other : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        if (other.get() == entity) { continue; }

        if (!other->componentManager.GetComponentOfType<BoxCollider>()) { return; }

        if (auto* obbData = std::get_if<OBBDataBox>(&data)) {
            OBBDataBox prevData = std::get<OBBDataBox>(data);

            glm::vec2 uX = glm::vec2(cos(entity->getRotation()), -sin(entity->getRotation()));
            glm::vec2 uY = glm::vec2(sin(entity->getRotation()), cos(entity->getRotation()));

            data = OBBDataBox{prevData.size, prevData.offset, entity->getRotation(),
                           CalculateOBBCentrePoint(entity->getPosition(), prevData.offset),
                           {uX, uY}, prevData.size * 0.5f};
        }

        Intersects(entity, other.get());
    }
}

void Marmalade::ECS::BoxCollider::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::BoxCollider::Serialize(const Entity* entity) {
    nlohmann::json j;
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            j["size"]["x"] = colliderData.size.x;
            j["size"]["y"] = colliderData.size.y;
            j["offset"]["x"] = colliderData.offset.x;
            j["offset"]["y"] = colliderData.offset.y;
        }
    }, data);

    return j;
}

void Marmalade::ECS::BoxCollider::Deserialize(nlohmann::json json, Entity* entity) {
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            colliderData.size.x = json["size"]["x"].get<float>();
            colliderData.size.y = json["size"]["y"].get<float>();
            colliderData.offset.x = json["offset"]["x"].get<float>();
            colliderData.offset.y = json["offset"]["y"].get<float>();
        }
    }, data);
}

void Marmalade::ECS::BoxCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    glm::vec2 posA = self->getPosition();
    glm::vec2 posB = other->getPosition();

    auto* aabbA = GetCollisionData<AABBDataBox>();
    auto* aabbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<AABBDataBox>();

    auto* obbA = GetCollisionData<OBBDataBox>();
    auto* obbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<OBBDataBox>();

    if (aabbA && aabbB) {
        if (IntersectsAABB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                glm::vec2 collisionNorm = glm::normalize(posA - posB);

                glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * aabbA->size;
                glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * aabbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (obbA && obbB) {
        if (IntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                glm::vec2 collisionNorm = glm::normalize(posB - posA);

                glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * obbA->size;
                glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * obbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (aabbA && obbB) {
        if (AABBIntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                glm::vec2 collisionNorm = glm::normalize(posB - posA);

                glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * aabbA->size;
                glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * obbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (obbA && aabbB) {
        if (AABBIntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                glm::vec2 collisionNorm = glm::normalize(posB - posA);

                glm::vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * obbA->size;
                glm::vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * aabbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    }
}

bool Marmalade::ECS::BoxCollider::IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    auto* aabbA = GetCollisionData<AABBDataBox>();
    auto* aabbB = std::get_if<AABBDataBox>(&other.data);

    if (!aabbA || !aabbB) return false;

    glm::vec2 minA = posA + aabbA->offset;
    glm::vec2 maxA = minA + aabbA->size;

    glm::vec2 minB = posB + aabbB->offset;
    glm::vec2 maxB = minB + aabbB->size;

    return (minA.x < maxB.x && maxA.x > minB.x &&
            minA.y < maxB.y && maxA.y > minB.y);
}

bool Marmalade::ECS::BoxCollider::IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    auto* obbA = GetCollisionData<OBBDataBox>();
    auto* obbB = std::get_if<OBBDataBox>(&other.data);

    if (!obbA || !obbB) return false;

    float radiiA, radiiB;
    glm::mat2 R, AbsR;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            R[i][j] = glm::dot(obbA->u[i], obbB->u[j]);
        }
    }

    // Compute translation vector
    glm::vec2 t = obbB->c - obbA->c;
    t = glm::vec2(glm::dot(t, obbA->u[0]), glm::dot(t, obbA->u[1]));

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            AbsR[i][j] = glm::abs(R[i][j]) + FLT_EPSILON;
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

bool Marmalade::ECS::BoxCollider::AABBIntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) {
    auto* aabb = GetCollisionData<AABBDataBox>();
    auto* obb = std::get_if<OBBDataBox>(&other.data);

    if (!aabb || !obb) return false;

    glm::vec2 aabbCentre = posA + aabb->offset + aabb->size * 0.5f;
    glm::vec2 aabbHalfSize = aabb->size * 0.5f;

    glm::vec2 obbCentre = posB + obb->offset;
    glm::vec2 obbAxes[2] = {obb->u[0], obb->u[1]};
    glm::vec2 obbHalfSize = obb->e;

    glm::vec2 trans = obbCentre - aabbCentre;

    glm::vec2 testingAxes[4] {
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            obbAxes[0],
            obbAxes[1]
    };

    for (int i = 0; i < 4; ++i) {
        glm::vec2 axis = testingAxes[i];
        if(!TestAABBSeparation(axis, aabbCentre, aabbHalfSize, obbCentre, obbAxes, obbHalfSize, trans)) {
            return false;
        }
    }

    return true;
}

void Marmalade::ECS::BoxCollider::ShowBounds(const glm::vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<AABBDataBox>(&data)) {
        glm::vec2 min = entityPosition + aabbData->offset;
        glm::vec2 max = min + aabbData->size;

        ImVec2 screenMin = EditorViews::WorldToScreenSpace(min);
        ImVec2 screenMax = EditorViews::WorldToScreenSpace(max);

        ImGui::GetWindowDrawList()->AddRect(
                screenMin, screenMax,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 0.0f, 0.0f, 2.0f
        );
    } else if (auto* obbData = std::get_if<OBBDataBox>(&data)) {
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
            screenCorners[i] = EditorViews::WorldToScreenSpace(rotatedCorners[i]);
        }

        ImGui::GetWindowDrawList()->AddQuad(
                screenCorners[0], screenCorners[1], screenCorners[2], screenCorners[3],
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 2.0f
        );
    }
}

bool Marmalade::ECS::BoxCollider::TestAABBSeparation(const glm::vec2& axis, const glm::vec2& aabbCenter, const glm::vec2& aabbHalfSize, const glm::vec2& obbCenter, const glm::vec2* obbAxes, const glm::vec2& obbHalfSize, const glm::vec2& t) {
    float aabbProj = aabbHalfSize.x * glm::abs(glm::dot(glm::vec2(1, 0), axis)) +
                           aabbHalfSize.y * glm::abs(glm::dot(glm::vec2(0, 1), axis));

    float obbProj = obbHalfSize.x * glm::abs(glm::dot(obbAxes[0], axis)) +
                          obbHalfSize.y * glm::abs(glm::dot(obbAxes[1], axis));

    float trans = glm::abs(glm::dot(t, axis));

    return trans > (aabbProj + obbProj);
}

glm::vec2 Marmalade::ECS::BoxCollider::CalculateOBBCentrePoint(const glm::vec2& entityPosition, const glm::vec2& offset) {
    return entityPosition + offset;
}