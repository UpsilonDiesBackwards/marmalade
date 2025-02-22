
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

#include <glm/vec2.hpp>

namespace Marmalade::ECS {
    struct AABBData {
        glm::vec2 size;
        glm::vec2 offset;
    };

    struct OBBData {
        glm::vec2 size;
        glm::vec2 offset;
        float rotation;
    };

    class ColliderBase {
    public:
        std::variant<AABBData, OBBData> data;

        virtual void Intersects(Entity* self, Entity* other) = 0;

        virtual bool IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) = 0;
        virtual bool IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB, float rotation) = 0;

        virtual void ShowBounds(Entity* entity) = 0;
    };
}

#endif//MARMALADE_COLLIDERBASE_H
