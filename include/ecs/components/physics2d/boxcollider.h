
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

#ifndef MARMALADE_BOXCOLLIDER_H
#define MARMALADE_BOXCOLLIDER_H

#include "ecs/component.h"
#include "colliderbase.h"
#include "mathematics/vector.h"

#include <glm/vec2.hpp>

#include <nlohmann/json.hpp>

namespace Marmalade::ECS {
    /*
     * This handles the collision detection of an entity.
     *
     * It is a very rudimentary implementation of AABB box collision...
     * ... we ought to support more shapes like circles and capsules...
     * ... and additional collision methods such as OBB.
     * */

    class BoxCollider : public ColliderBase {
    public:
        BoxCollider() {
            name = "Box Collider",
            allowMultiple = true,
            data = AABBDataBox{{1.0f, 1.0f}, {0.0f, 0.0f}};
            categories = {"Physics"};
            description =
                    "Adds 2D box shaped collision bounds component\n"
                    "Supports both AABB and OBB collision detection based on entity orientation";
        }

        Marmalade::Mathematics::Vec2 test;

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void Intersects(Entity* self, Entity* other) override;

        bool IntersectsAABB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override;
        bool IntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override;
        bool AABBIntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) override;

        void ShowBounds(const Marmalade::Mathematics::Vec2& entityPosition, Transform transform) override;

        bool TestAABBSeparation(const Marmalade::Mathematics::Vec2& axis, const Marmalade::Mathematics::Vec2& aabbCenter, const Marmalade::Mathematics::Vec2& aabbHalfSize, const Marmalade::Mathematics::Vec2& obbCenter, const Marmalade::Mathematics::Vec2* obbAxes, const Marmalade::Mathematics::Vec2& obbHalfSize, const Marmalade::Mathematics::Vec2& t) override;
        Marmalade::Mathematics::Vec2 CalculateOBBCentrePoint(const Marmalade::Mathematics::Vec2& entityPosition, const Marmalade::Mathematics::Vec2& offset) override;
    };

    REGISTER_COMPONENT(BoxCollider);
}

#endif//MARMALADE_BOXCOLLIDER_H
