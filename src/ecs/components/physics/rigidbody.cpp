
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
    if (isStatic) {
        velocity *= 0.0f;
        return;
    }

    auto deltaTime = static_cast<float>(Application::GetInstance().profiler.GetDeltaTime());

    _accumulator += deltaTime;

    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent jitteriness
        UpdatePhysics(entity, fixedTimeStep);
        _accumulator -= fixedTimeStep;
    }
}

void Marmalade::ECS::RigidBody::Setup(Entity* entity) {
}

nlohmann::json Marmalade::ECS::RigidBody::Serialize(const Entity* entity) {
    nlohmann::json j;
    j["static"] = isStatic;
    j["velocity"]["x"] = velocity.x;
    j["velocity"]["y"] = velocity.y;
    j["gravity"] = gravity;
    j["mass"] = mass;

    return j;
}

void Marmalade::ECS::RigidBody::Deserialize(nlohmann::json json, Entity* entity) {
    isStatic = json["static"].get<bool>();
    velocity.x = json["velocity"]["x"].get<float>();
    velocity.y = json["velocity"]["y"].get<float>();
    gravity = json["gravity"].get<float>();
    mass = json["mass"].get<float>();
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float time) {
    if (glm::length(velocity) < 0.01f) {
        velocity = glm::vec2(0.0f);
    }

    velocity.y += gravity * time;

    // Collision queue
    while (!collisionQueue.empty()) {
        CollisionEvent event = collisionQueue.front();
        collisionQueue.pop();

        Collide(event.self, event.other, event.normal);

        if (event.normal.y < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) {
            velocity.y = 0;
        }
    }

    momentum = mass * velocity;

    glm::vec2 newPos = entity->getPosition() + momentum * time;
    entity->setPosition(newPos);
}

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
void Marmalade::ECS::RigidBody::Collide(Entity* self, Entity* other, const glm::vec2 normal) {
    if (isStatic) return;

    auto* selfCollider = self->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherRigidBody = other->componentManager.GetComponentOfType<RigidBody>();

    if (!selfCollider || !otherCollider) return;

    glm::vec2 selfSize, otherSize;

    if (std::holds_alternative<AABBData>(selfCollider->data)) {
        selfSize = std::get<AABBData>(selfCollider->data).size;
    } else if (std::holds_alternative<OBBData>(selfCollider->data)) {
        selfSize = std::get<OBBData>(selfCollider->data).size;
    }

    if (std::holds_alternative<AABBData>(otherCollider->data)) {
        otherSize = std::get<AABBData>(otherCollider->data).size;
    } else if (std::holds_alternative<OBBData>(otherCollider->data)) {
        otherSize = std::get<OBBData>(otherCollider->data).size;
    }

    glm::vec2 overlapDist = self->getPosition() - other->getPosition();
    glm::vec2 combinedHalfHeight = selfSize * 0.5f + otherSize * 0.5f;
    overlapDist = combinedHalfHeight - glm::abs(overlapDist);

    glm::vec2 correction = normal * glm::max(glm::vec2(0.0f), overlapDist) * 0.37f;
    self->setPosition(self->getPosition() + correction);

    if (!otherRigidBody || otherRigidBody->isStatic) {
        velocity -= glm::dot(velocity, normal) * normal;
        return;
    }

    glm::vec2 combinedVelocities = velocity - otherRigidBody->velocity;
    float impulse = 2.0f * glm::dot(combinedVelocities, normal) / (mass + otherRigidBody->mass);
    glm::vec2 vectorImpulse = normal * impulse;

    ApplyImpulse(vectorImpulse);
    otherRigidBody->ApplyImpulse(-vectorImpulse);
}


void Marmalade::ECS::RigidBody::ApplyImpulse(glm::vec2 impulse) {
    if (mass == 0 || isStatic) return;

    velocity += impulse / mass;
}
