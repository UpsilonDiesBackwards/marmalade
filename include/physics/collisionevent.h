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

#ifndef MARMALADE_COLLISIONEVENT_H
#define MARMALADE_COLLISIONEVENT_H

#include <glm/vec3.hpp>
#include "scene/entity.h"

namespace Marmalade::Physics {
    /**
     * \struct CollisionEvent
     * \brief Represents the details of a collision between two entities.
     *
     * Contains information about whether a collision occurred, the collision normal,
     * penetration depth, the involved entities, and the contact point of collision.
     */
    struct CollisionEvent {
        bool colliding; //!< Returns true if two entities are currently colliding
        glm::vec3 normal; //!< The collision normal vector pointing from entity A to B
        float penetration; //!< The depth of the penetration (How much the colliders overlap)
        std::shared_ptr<Entity> A; //!< First entity involved in collision
        std::shared_ptr<Entity> B; //!< Second entity involved in collision
        // TODO Make this a vec3
        glm::vec2 contactPoint = {}; //!< Point of contact of the collision
    };
}

#endif
