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

#ifndef MARMALADE_RIGIDBODY_H
#define MARMALADE_RIGIDBODY_H

#include "../../component.h"

namespace Marmalade::ECS {
    /*!
     * \struct BodyT
     * \brief Generic physics body data structure.
     *
     * Represents the physical properties of a rigid body, including linear and angular
     * velocity, mass, momentum, and an associated collider. This is a pure data struct
     * used to define physics state that can be simulated by the physics system.
     *
     * \tparam VecType The vector type (e.g., glm::vec2 or glm::vec3).
     * \tparam ColliderType The collider type used (e.g., BoxCollider2D, SphereCollider3D).
     */
    template<typename VecType, typename ColliderType>
    struct BodyT {
        VecType velocity; //!< Linear Velocity of the body
        VecType centreOfMass; //!< Local centre of mass

        bool isStatic; //!< Determines if the body is static (immovable)
        float gravity = -9.81f; //!< Gravitational factor
        float mass = 1.0f; //!< Mass of the body
        float inverseMass = -mass; //!< Inverse mass

        float angularVelocity = 0.0f; //!< Angular velocity of the body
        float torque = 0.0f; //!< Torque applied to the body
        float angularMomentum = 0.0f; //!< Angular momentum
        float inertia = 1.0f; //!< Moment of Inertia
        float inverseInertia = -inertia; //!< Inverse moment of inertia

        VecType momentum; //!< Linear momentum
        float restitution = 0.0f; //!< Restitution coefficient  (additionally functions as friction)

        ColliderType collider; //!< Collider
    };

    /*!
     * \class RigidbodyT
     * \brief Generic component wrapper for a physics body.
     *
     * Templated base class that provides physics-related functionality to entities.
     * Used to construct components like Rigidbody2D or Rigidbody3D by specifying
     * vector and collider types.
     *
     * \tparam VecType The vector type (e.g., glm::vec2 or glm::vec3).
     * \tparam ColliderType The type of collider shape used.
     */
    template<typename VecType, typename ColliderType>
    class RigidbodyT : public Component {
    public:
        using Body = BodyT<VecType, ColliderType>;
        Body body;  //!< Rigidbody instance

        /*!
         * \brief Returns the current center of mass of the body.
         * \return The center of mass as a generic 'VecType'.
         */
        VecType GetCentreOfMass() const { return (body.centreOfMass); }

        void Display(Entity* entity) override = 0;
        void Apply(Entity* entity) override = 0;
        void Setup(Entity* entity) override = 0;
        nlohmann::json Serialize(const Entity* entity) override = 0;
        void Deserialize(nlohmann::json json, Entity* entity) override = 0;
    };
}

#endif //MARMALADE_RIGIDBODY2D_H