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
    ImGui::DragFloat2("Centre of Mass", &body.centreOfMass[0], 0.1f);
    ImGui::DragFloat2("Velocity", &body.velocity[0], 0.1f);
    ImGui::DragFloat("Gravity", &body.gravity, 0.1f);
    ImGui::DragFloat("Mass", &body.mass, 0.1f, 0.1f);
    ImGui::DragFloat("Elasticity", &body.elasticity, 0.0f);
}

void Marmalade::ECS::RigidBody::Apply(Entity* entity) {
    if (isStatic) {
        body.velocity *= 0.0f;
        return;
    }

    if (Application::GetInstance().playState != PlayState::PlayState_STEP &&
        Application::GetInstance().playState != PlayState::PlayState_PAUSE) {

        _accumulator = fixedTimeStep;
    } else {
        _accumulator += fixedTimeStep;
    }


    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent jitteriness
        if ((Application::GetInstance().playState == PlayState::PlayState_PLAY || // If the object is NOT static, and the application is in play or stepped...
             Application::GetInstance().playState == PlayState::PlayState_STEP) && !isStatic) {
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

    j["velocity"]["x"] = body.velocity[0];
    j["velocity"]["y"] = body.velocity[1];
    j["gravity"] = body.gravity;
    j["mass"] = body.mass;
    j["elasticity"] = body.elasticity;

    return j;
}

void Marmalade::ECS::RigidBody::Deserialize(nlohmann::json json, Entity* entity) {
    isStatic = json["static"].get<bool>();

    body.velocity[0] = json["velocity"]["x"].get<float>();
    body.velocity[1] = json["velocity"]["y"].get<float>();
    body.gravity = json["gravity"].get<float>();
    body.mass = json["mass"].get<float>();
    body.elasticity = json["elasticity"].get<float>();
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float time) {
    body.velocity[1] += body.gravity * time; // Increase Y velocity via gravity over time

    // Collision queue
    while (!collisionQueue.empty()) {
        CollisionEvent event = collisionQueue.front();
        collisionQueue.pop();

        Collide(event.self, event.other, event.normal, event.ptOnA_WorldSpace, event.ptOnB_WorldSpace);

        if (event.normal[1] < 0 && event.other->componentManager.GetComponentOfType<RigidBody>()->isStatic) {
            body.velocity[1] = 0;
        }

    }

    if (body.velocity.Magnitude() < FLT_EPSILON) {
        return;
    }

    momentum = Marmalade::Mathematics::Vec2(body.velocity[0] * body.mass, body.velocity[1] * body.mass);
    Marmalade::Mathematics::Vec2 newPos = entity->getPosition() + momentum * time; // Calculate the new desired position of the entity
    entity->setPosition(newPos);

    float angularDisplacement = body.angularVelocity * time;
    entity->setRotation(entity->getRotation() + (angularDisplacement * 5));
}

void Marmalade::ECS::RigidBody::Collide(Entity* self, Entity* other, const Marmalade::Mathematics::Vec2 normal, Marmalade::Mathematics::Vec2 ptOnA, Marmalade::Mathematics::Vec2 ptOnB) {
    if (isStatic) return;

    auto* selfCollider = self->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherCollider = other->componentManager.GetComponentOfType<ColliderBase>();
    auto* otherRigidBody = other->componentManager.GetComponentOfType<RigidBody>();

    ColliderInfo selfInfo = GetColliderInfo(selfCollider);
    ColliderInfo otherInfo = GetColliderInfo(otherCollider);

    Marmalade::Mathematics::Vec2 selfSize = selfInfo.size.value_or(Marmalade::Mathematics::Vec2(0.0f, 0.0f));
    Marmalade::Mathematics::Vec2 otherSize = otherInfo.size.value_or(Marmalade::Mathematics::Vec2(0.0f, 0.0f));

    // Calculate elasticity using the owning and other entities elasticity values
    float selfElasticity = body.elasticity;
    float otherElasticity = otherRigidBody->body.elasticity;
    float elasticity = selfElasticity * otherElasticity;

    if (Marmalade::Mathematics::Abs(normal[1]) > 0.99f) { // Set ptOnA/B[0] as centre of mass[0]
        ptOnA[0] = GetCentreOfMass()[0];
        ptOnB[0] = otherRigidBody->GetCentreOfMass()[0];
    }

    Marmalade::Mathematics::Vec2 ra = ptOnA - GetCentreOfMass();
    Marmalade::Mathematics::Vec2 rb = ptOnB - otherRigidBody->GetCentreOfMass();

    float crossRA_N = ra[0] * normal[1] - ra[1] * normal[0];
    float crossRB_N = rb[0] * normal[1] - rb[1] * normal[0];

    float angularA = GetInverseInertiaTensor(self) * (crossRA_N * crossRA_N);
    float angularB = GetInverseInertiaTensor(other) * (crossRB_N * crossRB_N);
    float angularFactor = (angularA + angularB); // Calculate angular factor

    Marmalade::Mathematics::Vec2 selfVelocity = Marmalade::Mathematics::Vec2(-ra[1], ra[0]) * body.velocity + body.angularVelocity;
    Marmalade::Mathematics::Vec2 otherVelocity = Marmalade::Mathematics::Vec2(-ra[1], ra[0]) * otherRigidBody->body.velocity + otherRigidBody->body.angularVelocity;

    Marmalade::Mathematics::Vec2 combinedVelocities = selfVelocity - otherVelocity; // Combine the velocities of the two objects

    // Calculate the base impulse...
    float impulse = -(1.0f + elasticity) * combinedVelocities.Dot(normal) /
                    (body.mass + otherRigidBody->body.mass + angularFactor);
    Marmalade::Mathematics::Vec2 vectorImpulse = normal * impulse; //... then use it, and the normal to calculate the vector impulse

   Marmalade::Mathematics::Vec2 penetrationDepth = (selfSize / 2.0f + otherSize / 2.0f) - Marmalade::Mathematics::Abs(self->getPosition() - other->getPosition());

   float restitutionFactor = 0.01f;
   Marmalade::Mathematics::Vec2 restitution = normal * std::max(penetrationDepth.Magnitude(), 0.0f) * restitutionFactor;

   restitution = restitution.Normalise() * std::min(restitution.Magnitude(), 2.0f);

   Marmalade::Mathematics::Vec2 perpendicular = Marmalade::Mathematics::Vec2(-normal[1], normal[0]).Normalise() * 0.5f;
   Marmalade::Mathematics::Vec2 contactPoint = self->getPosition() + normal * (selfSize / 2.0f) + perpendicular;

    ApplyImpulse(contactPoint, vectorImpulse, self); // Apply an impulse to self...
    if (!otherRigidBody->isStatic) { //... and if the other entity is NOT static, ...
        otherRigidBody->ApplyImpulse(contactPoint, -vectorImpulse, self); //... then apply the opposite vector impulse (we abide by the third law of motion here)
    }

    if (otherRigidBody->isStatic) { // If the other entity is static...
        self->setPosition(self->getPosition() + restitution); //... Only set the direction of the self
    } else { //... or if not
        float totalMass = body.mass + otherRigidBody->body.mass; // Calculate the total mass of the two entity
        float selfMove = otherRigidBody->body.mass / totalMass;
        float otherMove = body.mass / totalMass;

        // And set the position of both entities
        self->setPosition(self->getPosition() + restitution * selfMove);
        other->setPosition(other->getPosition() + restitution * otherMove);
    }

    const float velocityThreshold = FLT_EPSILON; // If the velocity is less than the threshold, then set the velocity to zero
    if (penetrationDepth.Magnitude() > velocityThreshold) {
        body.velocity = Marmalade::Mathematics::Vec2(0.0f, 0.0f);
    }

    // Set angular velocity to zero
    if (Marmalade::Mathematics::Abs(normal[1]) > 0.99f && otherRigidBody->isStatic) {
        body.angularVelocity = 0.0f;
    }
}

void Marmalade::ECS::RigidBody::ApplyImpulse(Marmalade::Mathematics::Vec2 point, Marmalade::Mathematics::Vec2 impulse, Entity* self) {
    if (body.mass == 0.0f) return;

    ApplyImpulseLinear(impulse);

    // Use Centre of Mass to determine the torque
    Marmalade::Mathematics::Vec2 position = GetCentreOfMass();
    Marmalade::Mathematics::Vec2 r = point - position;

    float torque = -r.Cross2D(r, impulse) * 0.2f;

    ApplyImpulseAngular(torque, self);
}

void Marmalade::ECS::RigidBody::ApplyImpulseLinear(Marmalade::Mathematics::Vec2 impulse) {
    if (body.mass == 0 || isStatic) return;

    body.velocity += impulse / body.mass; // Apply impulse velocity
}

void Marmalade::ECS::RigidBody::ApplyImpulseAngular(float dL, Entity* self) {
    if (body.mass == 0.0f) return;

    float invInertia = GetInverseInertiaTensor(self);

    if (Marmalade::Mathematics::Abs(dL) > FLT_EPSILON) { body.angularVelocity += invInertia * dL; }

//    LOG_INFO("angular velocity: {}", body.angularVelocity);

    const float maxAngularSpeed = 15.0f; // Limit angular speed to prevent it going haywire
    if (Marmalade::Mathematics::Abs(body.angularVelocity) > maxAngularSpeed) { // Set angular velocity
        body.angularVelocity = std::copysign(maxAngularSpeed, body.angularVelocity);
    }
}

float Marmalade::ECS::RigidBody::GetInertiaTensor(Entity* self) {
    auto* collider = self->componentManager.GetComponentOfType<ColliderBase>();
    ColliderInfo colliderInfo = GetColliderInfo(collider);

    float inertia = 0.0f;

    // Set inertia using data from used collider type
    std::visit([&](auto&& colliderData) {
        using T = std::decay_t<decltype(colliderData)>;

        if constexpr (std::is_same_v<T, DataCircle>) {
            float radius = colliderData.radius;
            inertia = 0.5f * body.mass * (radius * radius);
        } else if constexpr (std::is_same_v<T, AABBDataBox> || std::is_same_v<T, OBBDataBox>) {
            float width = colliderData.size[0];
            float height = colliderData.size[1];
            inertia = (1.0f / 12.0f) * body.mass * (width * width + height * height); // this line
        }
    }, collider->data);

    return inertia;
}

float Marmalade::ECS::RigidBody::GetInverseInertiaTensor(Entity* self) {
    float inertia = GetInertiaTensor(self);
    return (inertia > 0.0f) ? (1.0f / inertia) : 0.0f;
}
