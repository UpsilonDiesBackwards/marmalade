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

#include "../include/physics/2d/collisionevent2d.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

Marmalade::Physics::CollisionEvent Marmalade::Physics::CollisionEvents2D::BoxVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                                                                                   std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB) {
    CollisionEvent event;
    return event;
}

Marmalade::Physics::CollisionEvent Marmalade::Physics::CollisionEvents2D::CircleVsCircle(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                             std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB) {
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

    return event;
}

Marmalade::Physics::CollisionEvent Marmalade::Physics::CollisionEvents2D::CircleVsBox(std::shared_ptr<Entity> a, const ECS::Rigidbody2D& rbA,
                             std::shared_ptr<Entity> b, const ECS::Rigidbody2D& rbB) {
    CollisionEvent event;
    return event;
}