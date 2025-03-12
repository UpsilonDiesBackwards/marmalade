
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

namespace Marmalade::ECS {
    /*
     * This handles rigid physics resolution of a colliding entity.
     *
     * Any entity with a collider is REQUIRED to have this component attached...
     * ... perhaps we should implement a 'dependency' system where it will add a collider...
     * ....if it's not already attached?
     * */

    struct Body {

        glm::vec2 velocity = {0.0f, 0.0f};
        float mass = 1.0f;
        float gravity = -9.81f;
        float elasticity = 0.0f;
    };

    struct CollisionEvent {
        Entity* self;
        Entity* other = nullptr;
        glm::vec2 normal;
    };

    class RigidBody : public Component {
    public:
        std::queue<CollisionEvent> collisionQueue;

        bool isStatic = true;

        // Perhaps make this a struct?
        Body body;

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        void UpdatePhysics(Entity* entity, float fixedDelta);

        void Collide(Entity* self, Entity* other, const glm::vec2 normal);

        void ApplyImpulse(glm::vec2 impulse);

        RigidBody() {
            name = "Rigid Body";
            categories = {"Physics"};
        }

    private:
        // Accumulator for phys updates
        float _accumulator = 0.0f;
        const float fixedTimeStep = 1.0f / 60.0f;// Target is 60 updates per second

        glm::vec2 momentum;
    };

    REGISTER_COMPONENT(RigidBody);
}

#endif//MARMALADE_RIGIDBODY_H
