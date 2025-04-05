
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
#include "mathematics/vector.h"

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

        Marmalade::Mathematics::Vec2 uX = Marmalade::Mathematics::Vec2(cos(rotation), sin(rotation));
        Marmalade::Mathematics::Vec2 uY = Marmalade::Mathematics::Vec2(-uX[1], uX[0]);

        data = OBBDataBox{prevData.size, prevData.offset, rotation,
                       prevData.offset, {uX, uY}, prevData.size * 0.5f};
    }

    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            ImGui::DragFloat2("Size", colliderData.size.ToPtr(), 0.1f);
            ImGui::DragFloat2("Offset", colliderData.offset.ToPtr(), 0.1f);
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

            Marmalade::Mathematics::Vec2 uX = Marmalade::Mathematics::Vec2(cos(entity->getRotation()), -sin(entity->getRotation()));
            Marmalade::Mathematics::Vec2 uY = Marmalade::Mathematics::Vec2(sin(entity->getRotation()), cos(entity->getRotation()));

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
            j["size"]["x"] = colliderData.size[0];
            j["size"]["y"] = colliderData.size[1];
            j["offset"]["x"] = colliderData.offset[0];
            j["offset"]["y"] = colliderData.offset[1];
        }
    }, data);

    return j;
}

void Marmalade::ECS::BoxCollider::Deserialize(nlohmann::json json, Entity* entity) {
    std::visit([&](auto &colliderData) {
        using T = std::decay_t<decltype(colliderData)>;
        if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            colliderData.size[0] = json["size"]["x"].get<float>();
            colliderData.size[1] = json["size"]["y"].get<float>();
            colliderData.offset[0] = json["offset"]["x"].get<float>();
            colliderData.offset[1] = json["offset"]["y"].get<float>();
        }
    }, data);
}

void Marmalade::ECS::BoxCollider::Intersects(Entity* self, Entity* other) {
    if (!self || !other) return;

    Marmalade::Mathematics::Vec2 posA = self->getPosition();
    Marmalade::Mathematics::Vec2 posB = other->getPosition();

    auto* aabbA = GetCollisionData<AABBDataBox>();
    auto* aabbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<AABBDataBox>();

    auto* obbA = GetCollisionData<OBBDataBox>();
    auto* obbB = other->componentManager.GetComponentOfType<BoxCollider>()->GetCollisionData<OBBDataBox>();

    if (aabbA && aabbB) {
        if (IntersectsAABB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                Marmalade::Mathematics::Vec2 norm = posA - posB;
                Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();

                Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * aabbA->size;
                Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * aabbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (obbA && obbB) {
        if (IntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                Marmalade::Mathematics::Vec2 norm = posA - posB;
                Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();

                Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * obbA->size;
                Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * obbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (aabbA && obbB) {
        if (AABBIntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                Marmalade::Mathematics::Vec2 norm = posA - posB;
                Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();

                Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * aabbA->size;
                Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * obbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    } else if (obbA && aabbB) {
        if (AABBIntersectsOBB(*other->componentManager.GetComponentOfType<ColliderBase>(), posA, posB)) {
            if (auto* rb = self->componentManager.GetComponentOfType<Marmalade::ECS::RigidBody>()) {
                Marmalade::Mathematics::Vec2 norm = posA - posB;
                Marmalade::Mathematics::Vec2 collisionNorm = norm.Normalise();

                Marmalade::Mathematics::Vec2 ptOnA_WorldSpace = self->getPosition() + collisionNorm * obbA->size;
                Marmalade::Mathematics::Vec2 ptOnB_WorldSpace = other->getPosition() - collisionNorm * aabbB->size;

                rb->collisionQueue.push({self, other, collisionNorm, ptOnA_WorldSpace, ptOnB_WorldSpace});
            }
        }
    }
}

bool Marmalade::ECS::BoxCollider::IntersectsAABB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) {
    auto* aabbA = GetCollisionData<AABBDataBox>();
    auto* aabbB = std::get_if<AABBDataBox>(&other.data);

    if (!aabbA || !aabbB) return false;

    Marmalade::Mathematics::Vec2 minA = posA + aabbA->offset;
    Marmalade::Mathematics::Vec2 maxA = minA + aabbA->size;

    Marmalade::Mathematics::Vec2 minB = posB + aabbB->offset;
    Marmalade::Mathematics::Vec2 maxB = minB + aabbB->size;

    return (minA[0]  < maxB[0]  && maxA[0]  > minB[0]  &&
            minA[1] < maxB[1] && maxA[1] > minB[1]);
}

bool Marmalade::ECS::BoxCollider::IntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) {
    auto* obbA = GetCollisionData<OBBDataBox>();
    auto* obbB = std::get_if<OBBDataBox>(&other.data);

    if (!obbA || !obbB) return false;

    float radiiA, radiiB;
    glm::mat2 R, AbsR;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            R[i][j] = obbA->u[i].Dot(obbB->u[j]);
        }
    }

    // Compute translation vector
    Marmalade::Mathematics::Vec2 t = obbB->c - obbA->c;
    t = Marmalade::Mathematics::Vec2(t.Dot(obbA->u[0]), t.Dot(obbA->u[1]));

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

bool Marmalade::ECS::BoxCollider::AABBIntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) {
    auto* aabb = GetCollisionData<AABBDataBox>();
    auto* obb = std::get_if<OBBDataBox>(&other.data);

    if (!aabb || !obb) return false;

    Marmalade::Mathematics::Vec2 aabbCentre = posA + aabb->offset + aabb->size * 0.5f;
    Marmalade::Mathematics::Vec2 aabbHalfSize = aabb->size * 0.5f;

    Marmalade::Mathematics::Vec2 obbCentre = posB + obb->offset;
    Marmalade::Mathematics::Vec2 obbAxes[2] = {obb->u[0], obb->u[1]};
    Marmalade::Mathematics::Vec2 obbHalfSize = obb->e;

    Marmalade::Mathematics::Vec2 trans = obbCentre - aabbCentre;

    Marmalade::Mathematics::Vec2 testingAxes[4] {
            Marmalade::Mathematics::Vec2(1.0f, 0.0f),
            Marmalade::Mathematics::Vec2(0.0f, 1.0f),
            obbAxes[0],
            obbAxes[1]
    };

    for (int i = 0; i < 4; ++i) {
        Marmalade::Mathematics::Vec2 axis = testingAxes[i];
        if(!TestAABBSeparation(axis, aabbCentre, aabbHalfSize, obbCentre, obbAxes, obbHalfSize, trans)) {
            return false;
        }
    }

    return true;
}

void Marmalade::ECS::BoxCollider::ShowBounds(const Marmalade::Mathematics::Vec2& entityPosition, Transform transform) {
    if (auto* aabbData = std::get_if<AABBDataBox>(&data)) {
        Marmalade::Mathematics::Vec2 min = entityPosition + aabbData->offset;
        Marmalade::Mathematics::Vec2 max = min + aabbData->size;

        ImVec2 screenMin = EditorViews::WorldToScreenSpace(min);
        ImVec2 screenMax = EditorViews::WorldToScreenSpace(max);

        ImGui::GetWindowDrawList()->AddRect(
                screenMin, screenMax,
                ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 0.0f, 0.0f, 2.0f
        );
    } else if (auto* obbData = std::get_if<OBBDataBox>(&data)) {
        obbData->c = entityPosition + obbData->offset;

        float theta = glm::radians(transform.rotation);

        Marmalade::Mathematics::Vec2 corners[4] = {
                {0, 0}, {obbData->size[0] , 0}, {obbData->size[0] , obbData->size[1]}, {0, obbData->size[1]}
        };

        Marmalade::Mathematics::Vec2 rotatedCorners[4];
        for (int i = 0; i < 4; ++i) {
            rotatedCorners[i] = obbData->c + Marmalade::Mathematics::Vec2(
                                                     corners[i][0] * cos(theta) - corners[i][1] * sin(theta),
                                                     corners[i][0] * sin(theta) + corners[i][1] * cos(theta)
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

bool Marmalade::ECS::BoxCollider::TestAABBSeparation(const Marmalade::Mathematics::Vec2& axis, const Marmalade::Mathematics::Vec2& aabbCenter, const Marmalade::Mathematics::Vec2& aabbHalfSize, const Marmalade::Mathematics::Vec2& obbCenter, const Marmalade::Mathematics::Vec2* obbAxes, const Marmalade::Mathematics::Vec2& obbHalfSize, const Marmalade::Mathematics::Vec2& t) {
    float aabbProj = aabbHalfSize[0] * Marmalade::Mathematics::Abs(Marmalade::Mathematics::Vec2(1, 0).Dot(axis)) +
                     aabbHalfSize[1] * Marmalade::Mathematics::Abs(Marmalade::Mathematics::Vec2(0, 1).Dot(axis));

    float obbProj = obbHalfSize[0] * Marmalade::Mathematics::Abs(obbAxes[0].Dot(axis)) +
                    obbHalfSize[1] * Marmalade::Mathematics::Abs(obbAxes[1].Dot(axis));


    float trans = Marmalade::Mathematics::Abs(t.Dot(axis));

    return trans > (aabbProj + obbProj);
}

Marmalade::Mathematics::Vec2 Marmalade::ECS::BoxCollider::CalculateOBBCentrePoint(const Marmalade::Mathematics::Vec2& entityPosition, const Marmalade::Mathematics::Vec2& offset) {
    return entityPosition + offset;
}