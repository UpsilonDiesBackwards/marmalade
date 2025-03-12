
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
    ImGui::DragFloat("Elasticity", &body.elasticity, 0.0f);
}

void Marmalade::ECS::RigidBody::Apply(Entity* entity) {
    if (isStatic) {
        body.velocity *= 0.0f;
        return;
    }

    auto deltaTime = static_cast<float>(Application::GetInstance().profiler.GetDeltaTime());

    _accumulator += deltaTime;

    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent jitteriness
        if (Application::GetInstance().playState == PlayState::Play && !isStatic) { // If the object is NOT static, and the application is in play...
            UpdatePhysics(entity, fixedTimeStep); //... activate the physics
        }
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
    j["elasticity"] = body.elasticity;

    return j;
}

void Marmalade::ECS::RigidBody::Deserialize(nlohmann::json json, Entity* entity) {
    isStatic = json["static"].get<bool>();

    body.velocity.x = json["velocity"]["x"].get<float>();
    body.velocity.y = json["velocity"]["y"].get<float>();
    body.gravity = json["gravity"].get<float>();
    body.mass = json["mass"].get<float>();
    body.elasticity = json["elasticity"].get<float>();
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float time) {
    if (glm::length(body.velocity) < 0.01f) { // If the velocity is less than 0.01...
        body.velocity = glm::vec2(0.0f); //... set the velocity to zero
    }

    body.velocity.y += body.gravity * time; // Increase Y velocity via gravity over time

    // Collision queue
    while (!collisionQueue.empty()) {
        CollisionEvent event = collisionQueue.front();
        collisionQueue.pop();

        Collide(event.self, event.other, event.normal); // Call 'Collide()' using the current collision event's data

        if (event.normal.y < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) { // If the normal is 0 or the other is static...
            body.velocity.y = 0; //... set velocity to 0
        }
    }

    momentum = body.mass * body.velocity;

    glm::vec2 newPos = entity->getPosition() + momentum * time; // Calculate the new desired position of the entity
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

    glm::vec2 selfSize, otherSize;

    if (std::holds_alternative<AABBDataBox>(selfCollider->data)) { // We want to get the owning entities size to calculate the half size later...
        selfSize = std::get<AABBDataBox>(selfCollider->data).size;
    } else if (std::holds_alternative<OBBDataBox>(selfCollider->data)) {
        selfSize = std::get<OBBDataBox>(selfCollider->data).size;
    }

    if (std::holds_alternative<AABBDataBox>(otherCollider->data)) { //... alongside the other entities size
        otherSize = std::get<AABBDataBox>(otherCollider->data).size;
    } else if (std::holds_alternative<OBBDataBox>(otherCollider->data)) {
        otherSize = std::get<OBBDataBox>(otherCollider->data).size;
    }

    // Calculate elasticity using the owning and other entities elasticity values
    float selfElasticity = body.elasticity;
    float otherElasticity = otherRigidBody->body.elasticity;
    float elasticity = selfElasticity * otherElasticity;

    glm::vec2 combinedVelocities = body.velocity - otherRigidBody->body.velocity; // Combine the velocities of the two objects

    // Calculate the base impulse...
    float impulse = -(1.0f + elasticity) * glm::dot(combinedVelocities, normal) / (body.mass + otherRigidBody->body.mass);
    glm::vec2 vectorImpulse = normal * impulse; //... then use it, and the normal to calculate the vector impulse

    // Calculate the collision penetration depth...
    glm::vec2 penetrationDepth = (selfSize / 2.0f + otherSize / 2.0f) - glm::abs(self->getPosition() - other->getPosition());
    glm::vec2 correction = normal * glm::max(glm::vec2(0), penetrationDepth) * 0.37f; //... and then use it to calculate a correction value

    ApplyImpulse(vectorImpulse); // Apply an impulse to self...
    if (!otherRigidBody->isStatic) { //... and if the other entity is NOT static, ...
        otherRigidBody->ApplyImpulse(-vectorImpulse); //... then apply the opposite vector impulse (we abide by the third law of motion here)
    }

    const float velocityThreshold = 0.0f; // If the velocity is less than the threshold, then set the velocity to zero
    if (glm::length(body.velocity) < velocityThreshold) {
        body.velocity = glm::vec2(0.0f);
    }

    if (otherRigidBody->isStatic) { // If the other entity is static...
        self->setPosition(self->getPosition() + correction); //... Only set the direction of the self
    } else { //... or if not
        float totalMass = body.mass + otherRigidBody->body.mass; // Calculate the total mass of the two entity
        float selfMove = otherRigidBody->body.mass / totalMass;
        float otherMove = body.mass / totalMass;

        // And set the position of both entities
        self->setPosition(self->getPosition() + correction * selfMove);
        other->setPosition(other->getPosition() + correction * otherMove);
    }
}

void Marmalade::ECS::RigidBody::ApplyImpulse(glm::vec2 impulse) {
    if (body.mass == 0 || isStatic) return;

    body.velocity += impulse / body.mass; // Apply impulse velocity
}
