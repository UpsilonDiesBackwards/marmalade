
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

#include "ecs/components/physics2d/rigidbody.h"

#include <imgui.h>

void Marmalade::ECS::RigidBody::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::Checkbox("Static", &isStatic);
    ImGui::DragFloat2("Velocity", &body.velocity.x, 0.1f);
    ImGui::DragFloat("Gravity", &body.gravity, 0.1f);
    ImGui::DragFloat("Mass", &body.mass, 0.1f);
}

void Marmalade::ECS::RigidBody::Apply(Entity* entity) {
    if (isStatic) {
        body.velocity *= 0.0f;
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
    j["velocity"]["x"] = body.velocity.x;
    j["velocity"]["y"] = body.velocity.y;
    j["gravity"] = body.gravity;
    j["mass"] = body.mass;

    return j;
}

void Marmalade::ECS::RigidBody::Deserialize(nlohmann::json json, Entity* entity) {
    isStatic = json["static"].get<bool>();
    body.velocity.x = json["velocity"]["x"].get<float>();
    body.velocity.y = json["velocity"]["y"].get<float>();
    body.gravity = json["gravity"].get<float>();
    body.mass = json["mass"].get<float>();
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float time) {
    if (glm::length(body.velocity) < 0.01f) {
        body.velocity = glm::vec2(0.0f);
    }

    body.velocity.y += body.gravity * time;

    // Collision queue
    while (!collisionQueue.empty()) {
        CollisionEvent event = collisionQueue.front();
        collisionQueue.pop();

        Collide(event.self, event.other, event.normal);

        if (event.normal.y < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) {
            body.velocity.y = 0;
        }
    }

    momentum = body.mass * body.velocity;

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

    if (std::holds_alternative<AABBDataBox>(selfCollider->data)) {
        selfSize = std::get<AABBDataBox>(selfCollider->data).size;
    } else if (std::holds_alternative<OBBDataBox>(selfCollider->data)) {
        selfSize = std::get<OBBDataBox>(selfCollider->data).size;
    }

    if (std::holds_alternative<AABBDataBox>(otherCollider->data)) {
        otherSize = std::get<AABBDataBox>(otherCollider->data).size;
    } else if (std::holds_alternative<OBBDataBox>(otherCollider->data)) {
        otherSize = std::get<OBBDataBox>(otherCollider->data).size;
    }

    glm::vec2 overlapDist = self->getPosition() - other->getPosition();
    glm::vec2 combinedHalfHeight = selfSize * 0.5f + otherSize * 0.5f;
    overlapDist = combinedHalfHeight - glm::abs(overlapDist);

    glm::vec2 correction = normal * glm::max(glm::vec2(0.0f), overlapDist) * 0.37f;
    self->setPosition(self->getPosition() + correction);

    if (!otherRigidBody || otherRigidBody->isStatic) {
        body.velocity -= glm::dot(body.velocity, normal) * normal;
        return;
    }

    glm::vec2 combinedVelocities = body.velocity - otherRigidBody->body.velocity;
    float impulse = 2.0f * glm::dot(combinedVelocities, normal) / (body.mass + otherRigidBody->body.mass);
    glm::vec2 vectorImpulse = normal * impulse;

    ApplyImpulse(vectorImpulse);
    otherRigidBody->ApplyImpulse(-vectorImpulse);
}


void Marmalade::ECS::RigidBody::ApplyImpulse(glm::vec2 impulse) {
    if (body.mass == 0 || isStatic) return;

    body.velocity += impulse / body.mass;
}
