
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
            data = AABBDataCircle{0.5f, {0.0f, 0.0f}};
            categories = {"Physics"};
        }

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void Intersects(Entity* self, Entity* other) override;
        bool IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) override;
        bool IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) override;
        glm::vec2 CalculateOBBCentrePoint(const glm::vec2& entityPosition, const glm::vec2& offset) override;
        void ShowBounds(const glm::vec2& entityPosition, Transform transform) override;

        float WorldRadiusToScreenScale(float radius);
    };

    REGISTER_COMPONENT(CircleCollider);
}


#endif//MARMALADE_CIRCLECOLLIDER_H
