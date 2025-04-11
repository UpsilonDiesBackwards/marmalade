
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

#include "ecs/component.h"

#include "boxcollider.h"

#include <nlohmann/json.hpp>

#include <queue>

using namespace Marmalade::Mathematics;

namespace Marmalade::ECS {
    /*
     * This handles rigid physics resolution of a colliding entity.
     *
     * Any entity with a collider is REQUIRED to have this component attached...
     * ... perhaps we should implement a 'dependency' system where it will add a collider...
     * ....if it's not already attached?
     * */

    struct Body {
        Vec2 centreOfMass = {0.5f, 0.5f};
        Vec2 velocity = {0.0f, 0.0f};
        float angularVelocity = 0.0f;
        float mass = 1.0f;
        float gravity = -9.81f;
        float elasticity = 0.0f;
    };

    struct CollisionEvent {
        Entity* self;
        Entity* other = nullptr;
        Vec2 normal;

        Vec2 ptOnA_WorldSpace;
        Vec2 ptOnB_WorldSpace;
        Vec2 ptOnA_LocalSpace;
        Vec2 ptOnB_LocalSpace;
    };

    class RigidBody : public Component {
    public:
        std::queue<CollisionEvent> collisionQueue;

        bool isStatic = true;

        Body body;

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void UpdatePhysics(Entity* entity, float fixedDelta);

        void Collide(Entity* self, Entity* other, const Vec2 normal, Vec2 ptOnA, Vec2 ptOnB);

        void ApplyImpulse(Vec2 point, Vec2 impulse, Entity* self);
        void ApplyImpulseLinear(Vec2 impulse);
        void ApplyImpulseAngular(float dL, Entity* self);

        Vec2 GetCentreOfMass() { return body.centreOfMass; }
        float GetInertiaTensor(Entity* self);
        float GetInverseInertiaTensor(Entity* self);

        RigidBody() {
            name = "Rigid Body";
            categories = {"Physics"};
            description =
                    "Adds a 2D Rigid Body component\n"
                    "Allows an entity to be a physical object within the game world and to be affected by gravity, elasticity, and other parameters";
        }

    private:
        // Accumulator for phys updates
        float _accumulator = 0.0f;
        const float fixedTimeStep = 1.0f / 60.0f;// Target is 60 updates per second

        const float _velocityThreshold = FLT_EPSILON;

        Vec2 momentum;
    };

    REGISTER_COMPONENT(RigidBody);
}

#endif//MARMALADE_RIGIDBODY_H
