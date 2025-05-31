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

#include "../include/physics/2d/physicsengine2d.h"

#include "ecs/components/physics/2d/rigidbody2d.h"

#include "../../application/application.h"

#include "../../../include/physics/2d/physics2dutil.h"
#include "physics/collisionevent.h"
#include "physics/2d/collisionevent2d.h"

void Marmalade::Physics::PhysicsEngine2D::Update() {
    if (GET_APP.playState != PlayState_PLAY && GET_APP.playState != PlayState_STEP) { return; }

    for (auto& entity: GET_APP.sceneManager.GetCurrentScene()->GetEntities()) {
        if (!entity->componentManager.GetComponentOfType<ECS::Rigidbody2D>()) { continue; }
        auto& rb = entity->componentManager.GetComponentOfType<ECS::Rigidbody2D>()->body;

        if (!rb.isStatic) { rb.velocity.y += rb.gravity * GET_FIXED_DELTA_TIME; }
    }

    EulerIntegration();
    BroadPhase();
    NarrowPhase();
    Resolve();
}

Marmalade::Physics::PhysicsEngine2D::~PhysicsEngine2D() {}

void Marmalade::Physics::PhysicsEngine2D::EulerIntegration() {
    for (auto& entity: GET_APP.sceneManager.GetCurrentScene()->GetEntities()) {
        if (!entity->componentManager.GetComponentOfType<ECS::Rigidbody2D>()) { continue; }
        auto& rb = entity->componentManager.GetComponentOfType<ECS::Rigidbody2D>()->body;

        if (!rb.isStatic) {
            rb.momentum = rb.mass * rb.velocity;

            glm::vec3 newPos = glm::vec3(entity->getPosition().x, entity->getPosition().y, 0.0f) +
                               glm::vec3(rb.momentum * GET_FIXED_DELTA_TIME, 0.0f);
            entity->setPosition(newPos);
        }
    }
}

std::vector<std::pair<std::shared_ptr<Entity>, std::shared_ptr<Entity>>> _possibleCollisions;

void Marmalade::Physics::PhysicsEngine2D::BroadPhase() {
    _possibleCollisions.clear();

    for (size_t i = 0; i < GET_APP.sceneManager.GetCurrentScene()->GetEntities().size(); ++i) {
        for (size_t j = i + 1; j < GET_APP.sceneManager.GetCurrentScene()->GetEntities().size(); ++j) {
            auto a = GET_APP.sceneManager.GetCurrentScene()->GetEntities()[i];
            auto b = GET_APP.sceneManager.GetCurrentScene()->GetEntities()[j];

            AABB aabbA = GetAABB(a);
            AABB aabbB = GetAABB(b);

            if (AABBsIntersect(aabbA, aabbB)) { _possibleCollisions.emplace_back(a, b); }
        }
    }
}

std::vector<Marmalade::Physics::CollisionEvent> _collisions;

void Marmalade::Physics::PhysicsEngine2D::NarrowPhase() {
    _collisions.clear();
    for (auto& [a, b]: _possibleCollisions) {
        auto event = DetectCollision(a, b);
        if (event.colliding) { _collisions.push_back(event); }
    }
}

void Marmalade::Physics::PhysicsEngine2D::Resolve() {
    for (auto& collision: _collisions) {
        ResolveCollision(collision);
        PositionCorrection(collision);
    }
}

void Marmalade::Physics::PhysicsEngine2D::ResolveCollision(CollisionEvent event) {
    auto& a = event.A;
    auto& b = event.B;

    auto rbA = a->componentManager.GetComponentOfType<ECS::Rigidbody2D>();
    auto rbB = b->componentManager.GetComponentOfType<ECS::Rigidbody2D>();

    auto& bodyA = rbA->body;
    auto& bodyB = rbB->body;

    if (bodyA.isStatic && bodyB.isStatic) return;

    glm::vec2 relativeVelocity = bodyB.velocity - bodyA.velocity;

    float velAlongNormal = glm::dot(relativeVelocity, glm::vec2(event.normal));
    if (velAlongNormal > 0.0f) return;

    float e = std::min(bodyA.restitution, bodyB.restitution);

    float invMassA = bodyA.isStatic ? 0.0f : bodyA.inverseMass;
    float invMassB = bodyB.isStatic ? 0.0f : bodyB.inverseMass;

    float j = -(1.0f + e) * velAlongNormal;
    j /= (invMassA + invMassB);

    glm::vec2 impulse = j * event.normal;

    if (!bodyA.isStatic) bodyA.velocity -= impulse * invMassA;
    if (!bodyB.isStatic) bodyB.velocity += impulse * invMassB;
}

void Marmalade::Physics::PhysicsEngine2D::PositionCorrection(CollisionEvent event) {
    const float percent = 1.0f;
    const float slop = 0.01f;

    auto& a = event.A;
    auto& b = event.B;

    auto rbA = a->componentManager.GetComponentOfType<ECS::Rigidbody2D>();
    auto rbB = b->componentManager.GetComponentOfType<ECS::Rigidbody2D>();

    auto& bodyA = rbA->body;
    auto& bodyB = rbB->body;

    float invMassA = bodyA.isStatic ? 0.0f : bodyA.inverseMass;
    float invMassB = bodyB.isStatic ? 0.0f : bodyB.inverseMass;

    float correctionMagnitude = std::max(event.penetration - slop, 0.0f) / (invMassA + invMassB) * percent;
    glm::vec2 correction = correctionMagnitude * event.normal;

    if (!bodyA.isStatic) {
        glm::vec3 pos = a->getPosition();
        a->setPosition(pos - glm::vec3(correction * invMassA, 0.0f));
    }

    if (!bodyB.isStatic) {
        glm::vec3 pos = b->getPosition();
        b->setPosition(pos + glm::vec3(correction * invMassB, 0.0f));
    }
}

Marmalade::Physics::CollisionEvent Marmalade::Physics::PhysicsEngine2D::DetectCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    auto rbA = a->componentManager.GetComponentOfType<ECS::Rigidbody2D>();
    auto rbB = b->componentManager.GetComponentOfType<ECS::Rigidbody2D>();

    const auto& colA = rbA->body.collider;
    const auto& colB = rbB->body.collider;

    using CT = ColliderType2D;

    if (rbA->body.isStatic && rbB->body.isStatic) { return {}; }

    if (colA.type == CT::Box && colB.type == CT::Box) { return CollisionEvents2D::BoxVsBox(a, *rbA, b, *rbB); }

    if (colA.type == CT::Circle && colB.type == CT::Circle) { return CollisionEvents2D::CircleVsCircle(a, *rbA, b, *rbB); }

    if (colA.type == CT::Circle && colB.type == CT::Box) { return CollisionEvents2D::CircleVsBox(a, *rbA, b, *rbB); }

    if (colA.type == CT::Box && colB.type == CT::Circle) {
        auto result = CollisionEvents2D::CircleVsBox(a, *rbA, b, *rbB);
        result.normal = -result.normal;
    std:
        swap(result.A, result.B);
        return result;
    }

    return {};
}