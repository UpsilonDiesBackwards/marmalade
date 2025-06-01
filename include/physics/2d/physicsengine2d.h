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

#ifndef PHYSICSENGINE2D_H
#define PHYSICSENGINE2D_H

#include "../physicsengine.h"
#include "physics/collisionevent.h"

#include "scene/entity.h"

namespace Marmalade::Physics {
    /**
     * \class PhysicsEngine2D
     * \brief A 2D physics engine implementing core physics simulation steps.
     *
     * Inherits from the base PhysicsEngine class and overrides update functionality.
     * Provides utilities for 2D vector cross products, collision detection, resolution,
     * and physics integration.
     */
    class PhysicsEngine2D : PhysicsEngine {
    public:
        /**
         * \brief Update the physics simulation.
         *
         * This method overrides the base class update to perform physics steps including
         * integration, collision detection, and resolution.
         */
        void Update() override;
        ~PhysicsEngine2D() override;

        /**
         * \brief Calculate the cross product of a scalar and a 2D vector.
         *
         * \param a Scalar value.
         * \param v 2D vector.
         * \return A 2D vector perpendicular to v scaled by a.
         */
        glm::vec2 Cross(float a, const glm::vec2& v) {
            return glm::vec2(-a * v.y, a * v.x);
        }

        /**
         * \brief Calculate the scalar cross product of two 2D vectors.
         *
         * \param a First 2D vector.
         * \param b Second 2D vector.
         * \return The scalar cross product result.
         */
        float Cross(const glm::vec2& a, const glm::vec2& b) {
            return a.x * b.y - a.y * b.x;
        }

    private:
        /**
         * \brief Perform Euler integration to update positions and velocities.
         */
        void EulerIntegration();
        /**
         * \brief Perform broad phase collision detection to identify potential colliding pairs.
         */
        void BroadPhase();
        /**
         * \brief Perform narrow phase collision detection to confirm collisions and generate contact info.
         */
        void NarrowPhase();
        /**
         * \brief Resolve detected collisions by applying impulses and forces.
         */
        void Resolve();

        /**
         * \brief Resolve an individual collision event by applying collision response.
         *
         * \param event The collision event containing collision data.
         */
        void ResolveCollision(CollisionEvent event);

        /**
         * \brief Correct positional overlap to prevent sinking or jitter.
         *
         * \param event The collision event for which to perform position correction.
         */
        void PositionCorrection(CollisionEvent event);

        /**
         * \brief Detect collision between two entities.
         *
         * \param a Pointer to the first entity.
         * \param b Pointer to the second entity.
         * \return CollisionEvent describing the collision, if any.
         */
        CollisionEvent DetectCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    };
}

#endif //PHYSICSENGINE2D_H