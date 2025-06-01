// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_CIRCLECOLLIDER2D_H
#define MARMALADE_CIRCLECOLLIDER2D_H

#include <glm/vec2.hpp>
#include "ecs/component.h"
#include "ecs/components/transform.h"

namespace Marmalade::ECS {
    /*!
     * \class BoxCollider2D
     * \brief Represents a 2D circular collider component.
     *
     * This component adds an circular collider to an entity,
     * used for 2D physics-based collision detection. The collider is centered at
     * the entity's position plus an offset, and sized based on the `radius` member.
     *
     * It is intended to be used alongside a Rigidbody2D component to participate in collision handling.
     *
     * \author Marmalade Engine
     */
    class CircleCollider2D : public Component {
    public:
        /*!
         * \brief Radius of the circle collider.
         *
         * Defines the size of the circular shape.
         * Default value is 1.0.
         */
        float radius = 1.0f;

        /*!
         * \brief Positional offset of the circle collider.
         *
         * Offset is relative to the entity's position.
         * Useful for displacing the circle without modifying the transform.
         */
        glm::vec2 offset = {0.0f, 0.0f};

#define CIRCLECOLLIDER2D_CTOR_BODY              \
    name = "Circle Collider 2D";                \
    categories = {"Physics", "2D", "Collider"}; \
    description =                               \
            "Adds an circular collider shape\n" \
            "Used for collision detection as a rectangle centered at position + offset";

#ifdef _MSC_VER
        CircleCollider2D();
#else
        CircleCollider2D() { CIRCLECOLLIDER2D_CTOR_BODY }
#endif

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;
    };

    REGISTER_COMPONENT(CircleCollider2D);
}

#endif