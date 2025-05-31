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

//
// Created by tayler on 5/31/25.
//

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

struct AABB {
    glm::vec2 min;
    glm::vec2 max;
};

inline bool AABBsIntersect(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y);
}

inline AABB GetAABB(const std::shared_ptr<Entity> entity) {
    const glm::vec2& pos = entity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->pos;

    if (auto box = entity->componentManager.GetComponentOfType<Marmalade::ECS::BoxCollider2D>()) {
        glm::vec2 center = pos + box->offset;
        glm::vec2 halfSize = box->size * 0.5f;
        return { center - halfSize, center + halfSize };
    }

    if (auto circle = entity->componentManager.GetComponentOfType<Marmalade::ECS::CircleCollider2D>()) {
        glm::vec2 center = pos + circle->offset;
        float r = circle->radius;
        return { center - glm::vec2(r), center + glm::vec2(r) };
    }

    return {pos, pos};
}


#endif
