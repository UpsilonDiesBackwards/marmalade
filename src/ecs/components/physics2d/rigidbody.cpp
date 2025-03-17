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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void Marmalade::ECS::RigidBody::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::Checkbox("Static", &isStatic);
    ImGui::DragFloat2("Centre of Mass", &body.centreOfMass.x, 0.1f);
    ImGui::DragFloat2("Velocity", &body.velocity.x, 0.1f);
    ImGui::DragFloat("Gravity", &body.gravity, 0.1f);
    ImGui::DragFloat("Mass", &body.mass, 0.1f, 0.1f);
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

        Collide(event.self, event.other, event.normal, event.ptOnA_WorldSpace, event.ptOnB_WorldSpace); // Call 'Collide()' using the current collision event's data

        if (event.normal.y < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) { // If the normal is 0 or the other is static...
            body.velocity.y = 0; //... set velocity to 0
        }
    }

    momentum = body.mass * body.velocity;

    glm::vec2 newPos = entity->getPosition() + momentum * time; // Calculate the new desired position of the entity
    entity->setPosition(newPos);

    float angularDisplacement = body.angularVelocity * time;

    entity->setRotation(entity->getRotation() + (angularDisplacement * 5));
}

void Marmalade::ECS::RigidBody::Collide(Entity* self, Entity* other, const glm::vec2 normal, glm::vec2 ptOnA, glm::vec2 ptOnB) {
    if (isStatic) return;

    auto* selfCollider = self->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherRigidBody = other->componentManager.GetComponentOfType<RigidBody>();

    ColliderInfo selfInfo = GetColliderInfo(selfCollider);
    ColliderInfo otherInfo = GetColliderInfo(otherCollider);

    glm::vec2 selfSize = selfInfo.size.value_or(glm::vec2(0.0f));
    glm::vec2 otherSize = otherInfo.size.value_or(glm::vec2(0.0f));

    // Calculate elasticity using the owning and other entities elasticity values
    float selfElasticity = body.elasticity;
    float otherElasticity = otherRigidBody->body.elasticity;
    float elasticity = selfElasticity * otherElasticity;

    if (glm::abs(normal.y) > 0.99f) { // Set ptOnA/B.x as centre of mass.x
        ptOnA.x = GetCentreOfMass().x;
        ptOnB.x = otherRigidBody->GetCentreOfMass().x;
    }

    glm::vec2 ra = ptOnA - GetCentreOfMass();
    glm::vec2 rb = ptOnB - otherRigidBody->GetCentreOfMass();

    float crossRA_N = ra.x * normal.y - ra.y * normal.x;
    float crossRB_N = rb.x * normal.y - rb.y * normal.x;

    float angularA = GetInverseInertiaTensor(self) * (crossRA_N * crossRA_N);
    float angularB = GetInverseInertiaTensor(other) * (crossRB_N * crossRB_N);
    float angularFactor = (angularA + angularB); // Calculate angular factor

    glm::vec2 selfVelocity = body.velocity + body.angularVelocity * glm::vec2(-ra.y, ra.x);
    glm::vec2 otherVelocity = otherRigidBody->body.velocity + otherRigidBody->body.angularVelocity * glm::vec2(-ra.y, ra.x);

    glm::vec2 combinedVelocities = selfVelocity - otherVelocity; // Combine the velocities of the two objects

    // Calculate the base impulse...
    float impulse = -(1.0f + elasticity) * glm::dot(combinedVelocities, normal) / (body.mass + otherRigidBody->body.mass + angularFactor);
    glm::vec2 vectorImpulse = normal * impulse; //... then use it, and the normal to calculate the vector impulse

    // Calculate the collision penetration depth...
    glm::vec2 penetrationDepth = (selfSize / 2.0f + otherSize / 2.0f) - glm::abs(self->getPosition() - other->getPosition());
    glm::vec2 correction = normal * glm::max(glm::vec2(0), penetrationDepth) * 0.37f; //... and then use it to calculate a correction value

    glm::vec2 perpendicular = glm::normalize(glm::vec2(-normal.y, normal.x)) * 0.1f;
    glm::vec2 contactPoint = self->getPosition() + normal * (selfSize * 0.5f) + perpendicular;

    ApplyImpulse(contactPoint, vectorImpulse, self); // Apply an impulse to self...
    if (!otherRigidBody->isStatic) { //... and if the other entity is NOT static, ...
        otherRigidBody->ApplyImpulse(contactPoint, -vectorImpulse, self); //... then apply the opposite vector impulse (we abide by the third law of motion here)
    }

    const float velocityThreshold = FLT_EPSILON; // If the velocity is less than the threshold, then set the velocity to zero
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

    // Set angular velocity to zero (TODO: Object should rest flush on the object. This just stops it.)
    if (glm::abs(normal.y) > 0.99f && otherRigidBody->isStatic) {
        body.angularVelocity = 0.0f;
    }
}

void Marmalade::ECS::RigidBody::ApplyImpulse(glm::vec2 point, glm::vec2 impulse, Entity* self) {
    if (body.mass == 0.0f) return;

    ApplyImpulseLinear(impulse);

    // Use Centre of Mass to determine the torque
    glm::vec2 position = GetCentreOfMass();
    glm::vec2 r = point - position;
    float torque = -glm::cross(glm::vec3(r, 0), glm::vec3(impulse, 0)).z * 0.2f;

    glm::vec2 r = point - self->getPosition();
    float dL = r.x * impulse.y - r.y * impulse.x;

    ApplyImpulseAngular(torque, self);
}

void Marmalade::ECS::RigidBody::ApplyImpulseLinear(glm::vec2 impulse) {
    if (body.mass == 0 || isStatic) return;

    body.velocity += impulse / body.mass; // Apply impulse velocity
}

void Marmalade::ECS::RigidBody::ApplyImpulseAngular(float dL, Entity* self) {
    if (body.mass == 0.0f) return;

    float invInertia = GetInverseInertiaTensor(self);

    if (glm::abs(dL) > FLT_EPSILON) { body.angularVelocity += invInertia * dL; }

//    spdlog::info("angular velocity: {}", body.angularVelocity);

    const float maxAngularSpeed = 15.0f; // Limit angular speed to prevent it going haywire
    if (glm::sqrt(glm::length(body.angularVelocity)) > maxAngularSpeed) { // Set angular velocity
        body.angularVelocity = glm::sign(body.angularVelocity) * maxAngularSpeed;
    }
}

float Marmalade::ECS::RigidBody::GetInertiaTensor(Entity* self) {
    auto* collider = self->componentManager.GetComponentOfType<ColliderBase>();
    ColliderInfo colliderInfo = GetColliderInfo(collider);

    float inertia = 0.0f;

    // Set inertia using data from used collider type
    std::visit([&](auto&& colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, AABBDataCircle>) {
            float radius = colliderData.radius;
            inertia = 0.5f * body.mass * (radius * radius);
        } else if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            float width = colliderData.size.x;
            float height = colliderData.size.y;
            inertia = (1.0f / 12.0f) * body.mass * (width * width + height * height); // this line
        }
    }, collider->data);

    return inertia;
}

float Marmalade::ECS::RigidBody::GetInverseInertiaTensor(Entity* self) {
    float inertia = GetInertiaTensor(self);
    return (inertia > 0.0f) ? (1.0f / inertia) : 0.0f;
}
