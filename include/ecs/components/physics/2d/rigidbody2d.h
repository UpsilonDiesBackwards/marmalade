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

#ifndef MARMALADE_RIGIDBODY2D_H
#define MARMALADE_RIGIDBODY2D_H

#include <glm/vec2.hpp>
#include "ecs/components/physics/rigidbody.h"
#include "../../../../../src/physics/2d/collider2d.h"

namespace Marmalade::ECS {
    /*!
    * \class Rigidbody2D
    * \brief 2D physics body component for entities in the Marmalade engine.
    *
    * Adds a 2D rigid body to an entity, enabling it to be influenced by forces,
    * collisions, gravity, and other physical interactions in the 2D game world.
    *
    * This component is typically used in conjunction with 2D colliders
    * (e.g. BoxCollider2D, CircleCollider2D) to define the shape of the body.
    *
    * \note This class derives from RigidbodyT specialized for 2D with glm::vec2 and Collider2D.
    */
    class Rigidbody2D : public RigidbodyT<glm::vec2, Physics::Collider2D> {
    public:

#define RIGIDBODY2D_CTOR_BODY                  \
    name = "Rigidbody 2D";                     \
    categories = {"Physics", "2D"};            \
    description =                              \
            "Adds a 2D Rigid Body component\n" \
            "Allows an entity to be a physical object within the game world and to be affected by gravity, elasticity, and other parameters";

#ifdef _MSC_VER
        Rigidbody2D();
#else
        Rigidbody2D() {
            RIGIDBODY2D_CTOR_BODY
        }
#endif

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;
    };

    REGISTER_COMPONENT(Rigidbody2D);
}

#endif
