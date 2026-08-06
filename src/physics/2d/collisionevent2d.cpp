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

#include "collisionevent2d.h"
#include "ecs/components/physics/2d/boxcollider2d.h"
#include "physics2dutil.h"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::BoxVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D &rbA,
                                                std::shared_ptr<Entity> b, const ECS::Rigidbody2D &rbB) {
    CollisionEvent event;
    event.colliding = false;


    glm::vec2 posA = glm::vec2(a->getPosition().x, a->getPosition().y);
    glm::vec2 posB = glm::vec2(b->getPosition().x, b->getPosition().y);

    auto boxA = std::static_pointer_cast<BoxCollider2D>(rbA.body.collider.shape);
    auto boxB = std::static_pointer_cast<BoxCollider2D>(rbB.body.collider.shape);

    if (!boxA || !boxB) {
        return event;
    }

    glm::vec2 halfSizeA = boxA->size * 0.5f;
    glm::vec2 halfSizeB = boxB->size * 0.5f;

    glm::vec2 minA = posA + boxA->offset - halfSizeA;
    glm::vec2 maxA = posA + boxA->offset + halfSizeA;

    glm::vec2 minB = posB + boxB->offset - halfSizeB;
    glm::vec2 maxB = posB + boxB->offset + halfSizeB;

    float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
    if (overlapX <= 0) return event;

    float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
    if (overlapY <= 0) return event;

    event.colliding = true;
    event.A = a;
    event.B = b;

    if (overlapX < overlapY) {
        event.penetration = overlapX;

        if (posA.x < posB.x) {
            event.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
        } else {
            event.normal = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    } else {
        event.penetration = overlapY;

        if (posA.y < posB.y) {
            event.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            event.normal = glm::vec3(0.0f, -1.0f, 0.0f);
        }
    }

    glm::vec2 contactMin = glm::max(minA, minB);
    glm::vec2 contactMax = glm::min(maxA, maxB);
    glm::vec2 contactCentre = 0.5f * (contactMin + contactMax);
    event.contactPoint = glm::vec3(contactCentre, 0.0f);

    return event;
}

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::CircleVsCircle(std::shared_ptr<Entity> a, const ECS::Rigidbody2D &rbA,
                                                      std::shared_ptr<Entity> b, const ECS::Rigidbody2D &rbB) {
    CollisionEvent event;
    event.A = a;
    event.B = b;

    const glm::vec2 posA = a->componentManager.GetComponentOfType<ECS::Transform>()->pos;
    const glm::vec2 posB = b->componentManager.GetComponentOfType<ECS::Transform>()->pos;

    auto circleA = std::static_pointer_cast<CircleCollider2D>(rbA.body.collider.shape);
    auto circleB = std::static_pointer_cast<CircleCollider2D>(rbB.body.collider.shape);

    glm::vec2 colliderPosA = posA + circleA->offset;
    glm::vec2 colliderPosB = posB + circleB->offset;

    glm::vec2 diff = colliderPosB - colliderPosA;
    float distSqr = glm::length2(diff);

    float radiusSum = circleA->radius + circleB->radius;
    if (distSqr > radiusSum * radiusSum) {
        event.colliding = false;
        return event;
    }

    float distance = sqrt(distSqr);
    event.colliding = true;

    if (distance != 0.0f) {
        event.penetration = radiusSum - distance;
        glm::vec2 normal2D = diff / distance;
        event.normal = glm::vec3(normal2D, 0.0f);
    } else {
        event.penetration = radiusSum;
        event.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec2 pointA = colliderPosA + glm::vec2(event.normal) * circleA->radius;
    glm::vec2 pointB = colliderPosB - glm::vec2(event.normal) * circleB->radius;
    event.contactPoint = 0.5f * (pointA + pointB);

    return event;
}

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::CircleVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D &rbA,
                                                   std::shared_ptr<Entity> b, const ECS::Rigidbody2D &rbB) {
    CollisionEvent event;
    event.colliding = false;

    auto circle = std::static_pointer_cast<CircleCollider2D>(rbA.body.collider.shape);
    auto box = std::static_pointer_cast<BoxCollider2D>(rbB.body.collider.shape);
    if (!circle || !box) return event;

    glm::vec2 circlePos = glm::vec2(a->getPosition()) + circle->offset;
    glm::vec2 boxPos = glm::vec2(b->getPosition()) + box->offset;

    float rot = glm::eulerAngles(b->componentManager.GetComponentOfType<ECS::Transform>()->rotation).z;
    glm::mat2 rotationMat = glm::mat2(glm::vec2(cos(rot), sin(rot)), glm::vec2(-sin(rot), cos(rot)));

    glm::vec2 localCirclePos = glm::transpose(rotationMat) * (circlePos - boxPos);
    glm::vec2 halfSize = box->size * 0.5f;

    glm::vec2 closestPoint = glm::clamp(localCirclePos, -halfSize, halfSize);
    glm::vec2 difference = localCirclePos - closestPoint;

    float distanceSquared = glm::dot(difference, difference);
    if (distanceSquared <= circle->radius * circle->radius) {
        event.colliding = true;
        event.A = a;
        event.B = b;

        // TODO: Improve accuracy of normal
        glm::vec2 normalLocal = glm::length(difference) > 0.0f ? glm::normalize(difference) : glm::vec2(1.0f, 0.0f);
        glm::vec2 normalWorld = rotationMat * normalLocal;

        event.normal = glm::vec3(normalWorld, 0.0f);
        event.penetration = circle->radius - glm::sqrt(distanceSquared);
    }

    glm::vec2 contactWorld = rotationMat * closestPoint + boxPos;
    event.contactPoint = contactWorld;

    return event;
}

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::OBBVsOBB(std::shared_ptr<Entity> a, const Marmalade::ECS::Rigidbody2D &rbA,
                                                std::shared_ptr<Entity> b, const Marmalade::ECS::Rigidbody2D &rbB) {

    CollisionEvent event;
    event.colliding = false;

    auto boxA = std::static_pointer_cast<BoxCollider2D>(rbA.body.collider.shape);
    auto boxB = std::static_pointer_cast<BoxCollider2D>(rbB.body.collider.shape);
    if (!boxA || !boxB) return event;

    glm::vec2 posA = glm::vec2(a->getPosition().x, a->getPosition().y);
    glm::vec2 posB = glm::vec2(b->getPosition().x, b->getPosition().y);

    float rotA = glm::eulerAngles(a->componentManager.GetComponentOfType<ECS::Transform>()->rotation).z;
    float rotB = glm::eulerAngles(b->componentManager.GetComponentOfType<ECS::Transform>()->rotation).z;

    // Calculate axes for OBB A
    glm::vec2 axisA0 = glm::vec2(cos(rotA), sin(rotA));
    glm::vec2 axisA1 = glm::vec2(-sin(rotA), cos(rotA));

    glm::vec2 axisB0 = glm::vec2(cos(rotB), sin(rotB));
    glm::vec2 axisB1 = glm::vec2(-sin(rotB), cos(rotB));

    OBB obbA{posA + boxA->offset, {axisA0, axisA1}, boxA->size * 0.5f};
    OBB obbB{posB + boxB->offset, {axisB0, axisB1}, boxB->size * 0.5f};

    if (IsOBBvsOBB(obbA, obbB)) {
        event.colliding = true;
        event.A = a;
        event.B = b;

        // TODO: calculate penetration depth and collision normal
        glm::vec2 diff = posB - posA;
        if (glm::length(diff) > 0.001f)
            event.normal = glm::vec3(glm::normalize(diff), 0.0f);
        else
            event.normal = glm::vec3(1.0f, 0.0f, 0.0f);

        // Todo Calculate penetration amount ;)
        event.penetration = 0.0f;
    }

    glm::vec2 centerA = posA + boxA->offset;
    glm::vec2 centerB = posB + boxB->offset;

    glm::vec2 pointA = centerA + glm::vec2(event.normal) * 0.5f;
    glm::vec2 pointB = centerB - glm::vec2(event.normal) * 0.5f;

    glm::vec2 contact = 0.5f * (pointA + pointB);
    event.contactPoint = glm::vec3(contact, 0.0f);

    return event;
}

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::OBBVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D &rbA,
                                                std::shared_ptr<Entity> b, const ECS::Rigidbody2D &rbB) {
    auto transform = b->componentManager.GetComponentOfType<ECS::Transform>();
    transform->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    return OBBVsOBB(a, rbA, b, rbB);
}

Marmalade::Physics::CollisionEvent
Marmalade::Physics::CollisionEvents2D::OBBVsCircle(std::shared_ptr<Entity> a, const ECS::Rigidbody2D &rbA,
                                                   std::shared_ptr<Entity> b, const ECS::Rigidbody2D &rbB) {
    auto result = CircleVsBox(b, rbB, a, rbA);
    if (result.colliding) {
        result.normal = -result.normal;
        std::swap(result.A, result.B);
    }
    return result;
}
