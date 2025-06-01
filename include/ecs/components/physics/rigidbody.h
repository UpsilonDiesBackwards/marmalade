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

#ifndef MARMALADE_RIGIDBODY_H
#define MARMALADE_RIGIDBODY_H

#include "ecs/component.h"

namespace Marmalade::ECS {
    template<typename VecType, typename ColliderType>
    struct BodyT {
        VecType velocity;
        VecType centreOfMass;

        bool isStatic;
        float gravity = -9.81f;
        float mass = 1.0f;
        float inverseMass = -mass;

        float angularVelocity = 0.0f;
        float torque = 0.0f;
        float angularMomentum = 0.0f;
        float inertia = 1.0f;
        float inverseInertia = -inertia;

        VecType momentum;
        float restitution = 0.0f;// Additionally functions as friction

        ColliderType collider;
    };

    template<typename VecType, typename ColliderType>
    class RigidbodyT : public Component {
    public:
        using Body = BodyT<VecType, ColliderType>;
        Body body;

        VecType GetCentreOfMass() const { return (body.centreOfMass); }

        void Display(Entity* entity) override = 0;
        void Apply(Entity* entity) override = 0;
        void Setup(Entity* entity) override = 0;
        nlohmann::json Serialize(const Entity* entity) override = 0;
        void Deserialize(nlohmann::json json, Entity* entity) override = 0;
    };
}

#endif //MARMALADE_RIGIDBODY2D_H