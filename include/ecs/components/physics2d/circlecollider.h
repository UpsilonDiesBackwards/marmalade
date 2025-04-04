
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

#ifndef MARMALADE_CIRCLECOLLIDER_H
#define MARMALADE_CIRCLECOLLIDER_H

#include "colliderbase.h"

namespace Marmalade::ECS {
    class CircleCollider : public ColliderBase {
    public:
        CircleCollider() {
            name = "Circle Collider",
            allowMultiple = true,
            data = DataCircle{0.5f, {0.0f, 0.0f}};
            categories = {"Physics"};
            description =
                    "Adds 2D circle shaped collision bounds component\n"
                    "Supports both AABB and OBB collision detection based on entity orientation";
        }

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void Intersects(Entity* self, Entity* other) override;

        void ShowBounds(const Marmalade::Mathematics::Vec2& entityPosition, Transform transform) override;

        static float WorldRadiusToScreenScale(float radius);

        // Unused pure virtual methods
        bool IntersectsAABB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override;
        bool IntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override {
            return false;
        }
        bool AABBIntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override {
            return false;
        }

        bool TestAABBSeparation(const Marmalade::Mathematics::Vec2& axis, const Marmalade::Mathematics::Vec2& aabbCenter, const Marmalade::Mathematics::Vec2& aabbHalfSize, const Marmalade::Mathematics::Vec2& obbCenter, const Marmalade::Mathematics::Vec2* obbAxes, const Marmalade::Mathematics::Vec2& obbHalfSize, const Marmalade::Mathematics::Vec2& t) override {
            return false;
        }
        Marmalade::Mathematics::Vec2 CalculateOBBCentrePoint(const Marmalade::Mathematics::Vec2& entityPosition, const Marmalade::Mathematics::Vec2& offset) override {
            return Marmalade::Mathematics::Vec2();
        }
    };

    REGISTER_COMPONENT(CircleCollider);
}

#endif//MARMALADE_CIRCLECOLLIDER_H
