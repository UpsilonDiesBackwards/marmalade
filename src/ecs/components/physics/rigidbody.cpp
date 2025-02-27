
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

    glm::vec2 overlap = self->getPosition() - other->getPosition();
    float overlapDist = glm::dot(overlap, normal);

    self->setPosition(self->getPosition() + normal * overlapDist);

    velocity *= 0.0;
}
