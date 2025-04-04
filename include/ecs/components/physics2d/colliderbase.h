
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

#ifndef MARMALADE_COLLIDERBASE_H
#define MARMALADE_COLLIDERBASE_H

#include "ecs/component.h"
#include "../../../../src/application/application.h"

#include "colliderdata.h"

#include <glm/vec4.hpp>

#include <variant>

namespace Marmalade::ECS {
    class ColliderBase : public Component {
    public:
        std::variant<AABBDataBox, OBBDataBox, DataCircle> data;

        bool showingBounds{false};

        virtual void Intersects(Entity* self, Entity* other) = 0;

        virtual bool IntersectsAABB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) = 0;
        virtual bool IntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) = 0;
        virtual bool AABBIntersectsOBB(const ColliderBase& other, const Marmalade::Mathematics::Vec2& posA, const Marmalade::Mathematics::Vec2& posB) = 0;

        virtual bool TestAABBSeparation(const Marmalade::Mathematics::Vec2& axis, const Marmalade::Mathematics::Vec2& aabbCenter, const Marmalade::Mathematics::Vec2& aabbHalfSize,
                                           const Marmalade::Mathematics::Vec2& obbCenter, const Marmalade::Mathematics::Vec2 obbAxes[2], const Marmalade::Mathematics::Vec2& obbHalfSize,
                                           const Marmalade::Mathematics::Vec2& t) = 0;
        virtual Marmalade::Mathematics::Vec2 CalculateOBBCentrePoint(const Marmalade::Mathematics::Vec2& entityPosition, const Marmalade::Mathematics::Vec2& offset) = 0;

        template<typename T>
        T* GetCollisionData() {
            return std::get_if<T>(&data);
        }

        virtual void ShowBounds(const Marmalade::Mathematics::Vec2& entityPosition, Transform transform) = 0;
    };
}

#endif
