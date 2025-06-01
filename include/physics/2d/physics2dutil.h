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

#ifndef MARMALADE_PHYSICS2DUTIL_H
#define MARMALADE_PHYSICS2DUTIL_H

#include <glm/vec2.hpp>

#include "../../ecs/components/physics/2d/boxcollider2d.h"
#include "../../ecs/components/physics/2d/rigidbody2d.h"
#include "ecs/components/transform.h"
#include "ecs/components/physics/2d/circlecollider2d.h"
#include "physics/2d/collider2d.h"

namespace Marmalade::ECS {
    class CircleCollider2D;
}

/**
 * \struct AABB
 * \brief Axis-Aligned Bounding Box structure for 2D collision.
 *
 * Represents a bounding box defined by minimum and maximum points in 2D space.
 */
struct AABB {
    glm::vec2 min; //!< Min (bottom-left) corner of the AABB
    glm::vec2 max; //!< Max (top-right) corner of the AABB
};

/**
 * \struct OBB
 * \brief Oriented Bounding Box structure for 2D collision.
 *
 * Represents a bounding box with a position, orientation axes, and half sizes.
 */
struct OBB {
    glm::vec2 center; //!< Centre point of the AABB
    glm::vec2 axes[2]; //!< Normalised axes
    glm::vec2 halfSize; //!< Half of the box's size
};

/**
 * \brief Check if two AABBs intersect.
 *
 * \param a The first AABB.
 * \param b The second AABB.
 * \return true if the AABBs overlap, false otherwise.
 */
inline bool AABBsIntersect(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y);
}

/**
 * \brief Get the axis-aligned bounding box (AABB) for an entity.
 *
 * Calculates the AABB based on the entity's position and its collider shape
 * (box or circle). Returns a degenerate AABB at the entity's position if no
 * collider is found.
 *
 * \param entity Shared pointer to the entity.
 * \return AABB representing the bounding box of the entity's collider.
 */
inline AABB GetAABB(const std::shared_ptr<Entity> entity) {
    const glm::vec2& pos = entity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->pos;

    if (auto box = entity->componentManager.GetComponentOfType<Marmalade::ECS::BoxCollider2D>()) {
        glm::vec2 center = pos + box->offset;
        glm::vec2 halfSize = box->size * 0.5f;
        return {center - halfSize, center + halfSize};
    }

    if (auto circle = entity->componentManager.GetComponentOfType<Marmalade::ECS::CircleCollider2D>()) {
        glm::vec2 center = pos + circle->offset;
        float r = circle->radius;
        return {center - glm::vec2(r), center + glm::vec2(r)};
    }

    return {pos, pos};
}


/**
 * \brief Check if two OBBs overlap on a specified axis.
 *
 * Projects both OBBs onto the given axis and checks if their projections overlap.
 *
 * \param a The first OBB.
 * \param b The second OBB.
 * \param axis The axis to test overlap on.
 * \return true if projections overlap on the axis, false otherwise.
 */
inline bool OverlapOnAxis(const OBB& a, const OBB& b, const glm::vec2& axis) {
    auto Project = [](const OBB& obb, const glm::vec2& axis) {
        float projectionCenter = glm::dot(obb.center, axis);
        float projectionRadius =
                obb.halfSize.x * std::abs(glm::dot(obb.axes[0], axis)) +
                obb.halfSize.y * std::abs(glm::dot(obb.axes[1], axis));
        return std::make_pair(projectionCenter - projectionRadius, projectionCenter + projectionRadius);
    };

    auto [minA, maxA] = Project(a, axis);
    auto [minB, maxB] = Project(b, axis);

    return !(maxA < minB || maxB < minA);
}

/**
 * \brief Check if two OBBs are intersecting.
 *
 * Uses the Separating Axis Theorem (SAT) by testing overlap along all axes
 * defined by both OBBs.
 *
 * \param a The first OBB.
 * \param b The second OBB.
 * \return true if the OBBs intersect, false otherwise.
 */
inline bool IsOBBvsOBB(const OBB& a, const OBB& b) {
    glm::vec2 axesToTest[4] = {a.axes[0], a.axes[1], b.axes[0], b.axes[1]};

    for (int i = 0; i < 4; ++i) { if (!OverlapOnAxis(a, b, axesToTest[i])) { return false; } }
    return true;
}

/**
 * \brief Compute the moment of inertia for a 2D collider shape.
 *
 * Calculates the rotational inertia of the collider based on its shape type
 * and mass. Supports box and circle colliders.
 *
 * \param collider The collider whose inertia to compute.
 * \param mass The mass of the entity owning the collider.
 * \return The moment of inertia for the collider.
 */
inline float ComputeInertia(const Marmalade::Physics::Collider2D& collider, float mass) {
    using namespace Marmalade::Physics;
    using CT = ColliderType2D;

    switch (collider.type) {
        case CT::Box: {
            const auto* box = static_cast<const BoxCollider2D*>(collider.shape.get());
            if (!box) return 0.0f;

            float width = box->size.x;
            float height = box->size.y;
            return (mass / 12.0f) * (width * width + height * height);
        }

        case CT::Circle: {
            const auto* circle = static_cast<const CircleCollider2D*>(collider.shape.get());
            if (!circle) return 0.0f;

            float radius = circle->radius;
            return 0.5f * mass * radius * radius;
        }

        default:
            return 0.0f;
    }
}

#endif