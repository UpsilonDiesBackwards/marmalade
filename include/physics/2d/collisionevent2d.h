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

#ifndef COLLISIONEVENTS2D_H
#define COLLISIONEVENTS2D_H

#include "ecs/components/physics/2d/rigidbody2d.h"
#include "physics/collisionevent.h"

namespace Marmalade::Physics {
    /**
     * \class CollisionEvents2D
     * \brief Provides static methods for detecting collisions between 2D physics shapes.
     *
     * This class contains static collision detection functions for various 2D collider types
     * including boxes, circles, and oriented bounding boxes (OBBs). Each function
     * returns a CollisionEvent struct that details whether a collision occurred and
     * relevant collision information such as contact points, penetration depth, and collision normals.
     */
    class CollisionEvents2D {
    public:

        /**
         * \brief Detects collision between two axis-aligned boxes (AABB).
         *
         * \param a Shared pointer to the first entity.
         * \param rbA The Rigidbody2D component of the first entity containing collider info.
         * \param b Shared pointer to the second entity.
         * \param rbB The Rigidbody2D component of the second entity containing collider info.
         * \return CollisionEvent Contains collision data and whether the two boxes overlap.
         */
        static CollisionEvent BoxVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                             std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

        /**
         * \brief Detects collision between two circles.
         *
         * \param a Shared pointer to the first entity.
         * \param rbA The Rigidbody2D component of the first entity containing collider info.
         * \param b Shared pointer to the second entity.
         * \param rbB The Rigidbody2D component of the second entity containing collider info.
         * \return CollisionEvent Contains collision data and whether the two circles overlap.
         */
        static CollisionEvent CircleVsCircle(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                             std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

        static CollisionEvent CircleVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                             std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

        static CollisionEvent OBBVsOBB(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                                       std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

        static CollisionEvent OBBVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                                       std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

        static CollisionEvent OBBVsCircle(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                                          std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB);

    };
}

#endif //COLLISIONEVENTS2D_H
