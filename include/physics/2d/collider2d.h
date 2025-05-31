
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

#ifndef MARMALADE_COLLIDER2D_H
#define MARMALADE_COLLIDER2D_H

#include <memory>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

namespace Marmalade::Physics {
    enum class ColliderType2D {
        Box,
        Circle,
        OBB
    };

    struct ShapeData2D {
        virtual ~ShapeData2D() = default;
    };

    struct BoxCollider2D : public ShapeData2D {
        glm::vec2 size = {1.0f, 1.0f};
        glm::vec2 offset = {0.0f, 0.0f};
    };

    struct CircleCollider2D : public ShapeData2D {
        float radius = 1.0f;
        glm::vec2 offset = {0.0f, 0.0f};
    };

    struct Collider2D {
        ColliderType2D type;
        std::shared_ptr<ShapeData2D> shape;
    };
}

#endif //MARMALADE_COLLIDER2D_H
