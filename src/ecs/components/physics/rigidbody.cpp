
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

#include "ecs/components/physics/rigidbody.h"

#include <imgui.h>

void Marmalade::ECS::RigidBody::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::Checkbox("Static", &isStatic);
    ImGui::DragFloat2("Velocity", &velocity.x, 0.1f);
    ImGui::DragFloat("Gravity", &gravity, 0.1f);
    ImGui::DragFloat("Mass", &mass, 0.1f);
}

void Marmalade::ECS::RigidBody::Apply(Entity* entity) {
    if (isStatic) return;

    auto deltaTime = static_cast<float>(Application::GetInstance().profiler.GetDeltaTime());

    _accumulator += deltaTime;

    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent jitteriness
        UpdatePhysics(entity, fixedTimeStep);
        _accumulator -= fixedTimeStep;
    }
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float deltaTime) {
    // Collision queue
    while (!collisionQueue.empty()) {
        CollisionEvent event = collisionQueue.front();
        collisionQueue.pop();

        Collide(event.self, event.other, event.normal);

        if (event.normal.y < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) {
            velocity.y = 0;
        }
    }

    velocity.y += gravity * deltaTime;

    momentum = mass * velocity;

    glm::vec2 newPos = entity->getPosition() + momentum * deltaTime;
    entity->setPosition(newPos);
}

void Marmalade::ECS::RigidBody::Collide(Entity* self, Entity* other, const glm::vec2 normal) {
    /*
     * This function is still very rudimentary and does not currently provide accurate physics collision.
     * currently when a moving (non-static) rigidbody collides with a stationary (static) rigidbody the
     * non-static entity will "bounce" on the surface of the static rigidbody. This is most likely due to
     * the collision normal being incorrectly computed in the Colliders.
     *
     * It seems that different collision detection methods have their own way of calculating a collision
     * normal.
     *
     * TODO: We should eventually add rotational force for when a rigidbody falls of the corner of a rb
     * */

    if (isStatic) return;

    // TODO: We should eventually add rotational force for when a rigidbody falls of the corner of a rb

    auto* selfCollider = self->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();

    if (!selfCollider || !otherCollider) return;

    glm::vec2 selfHalfHeight, otherHalfHeight;

    // SELF

    if (std::holds_alternative<AABBData>(selfCollider->data)) {
        selfHalfHeight = std::get<AABBData>(selfCollider->data).size / 2.0f;
    } else if (std::holds_alternative<OBBData>(selfCollider->data)) {
        selfHalfHeight = std::get<OBBData>(selfCollider->data).size / 2.0f;
    }

    // OTHER

    if (std::holds_alternative<AABBData>(otherCollider->data)) {
        otherHalfHeight = std::get<AABBData>(otherCollider->data).size / 2.0f;
    } else if (std::holds_alternative<OBBData>(otherCollider->data)) {
        otherHalfHeight = std::get<OBBData>(otherCollider->data).size / 2.0f;
    }

    glm::vec2 overlapDist = self->getPosition() - other->getPosition();
    glm::vec2 combinedHalfHeight = selfHalfHeight + otherHalfHeight;
    overlapDist = combinedHalfHeight - overlapDist;

    glm::vec2 correction = normal * glm::max(glm::vec2(0.0f), overlapDist);
    self->setPosition(self->getPosition() + correction);
    velocity -= glm::dot(velocity, normal) * normal;
}
