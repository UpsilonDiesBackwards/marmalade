
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
            name = "BoxCollider",
            allowMultiple = true,
            data = AABBData{{1.0f, 1.0f}, {0.0f, 0.0f} };
        }

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void Intersects(Entity* self, Entity* other) override;

        bool IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) override;
        bool IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) override;

        void ShowBounds(const glm::vec2& entityPosition, Transform transform) override;

        glm::vec2 CalculateOBBCentrePoint(const glm::vec2& entityPosition, const glm::vec2& offset) override;
    };

    REGISTER_COMPONENT(BoxCollider);
}

#endif//MARMALADE_BOXCOLLIDER_H
