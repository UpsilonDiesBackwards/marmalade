
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

#ifndef MARMALADE_COLLIDERDATA_H
#define MARMALADE_COLLIDERDATA_H

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include <optional>

namespace Marmalade::ECS {
struct AABBDataBox {
    glm::vec2 size;
    glm::vec2 offset;
};

struct OBBDataBox {
    glm::vec2 size;
    glm::vec2 offset;
    float rotation;
    glm::vec2 c;  // Center point
    glm::vec2 u[2];  // Local x, y axes
    glm::vec2 e;  // Half-width
};

struct AABBDataCircle {
    float radius;
    glm::vec2 offset;
};

struct ColliderInfo {
    std::optional<glm::vec2> offset;

    // Sizes
    std::optional<glm::vec2> size;
    std::optional<float> radius;

    // OBB Specific
    std::optional<float> rotation;
    std::optional<glm::vec2> center;
    std::optional<glm::vec2> axes[2];
    std::optional<glm::vec2> halfWidth;
};

    class ColliderBase;
    ColliderInfo GetColliderInfo(const ColliderBase* collider);
}

#endif
