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

#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include "../../gui/editor/editor.h"

#include "../../../include/ecs/components/transform.h"

namespace Marmalade::Physics {
    /*!
     * \enum ColliderType2D
     * \brief Enum for different 2D collider types.
     */
    enum class ColliderType2D {
        Box,
        Circle,
        OBB
    };

    /*!
     * \struct ShapeData2D
     * \brief Abstract base class for all 2D collider shapes.
     *
     * Provides a virtual interface for drawing collider shapes in the editor.
     */
    struct ShapeData2D {
        virtual void ShowBounds(const glm::vec2& entityPosition, Marmalade::ECS::Transform transform) = 0;
        virtual ~ShapeData2D() = default;
    };

    /*!
     * \struct BoxCollider2D
     * \brief Represents a 2D box collider shape.
     *
     * A rectangular collider with customizable size and offset. Can be rotated if
     * the entity's transform has rotation.
     */
    struct BoxCollider2D : public ShapeData2D {
        glm::vec2 size = {1.0f, 1.0f};//!< Width and height of the 2D Box Collider
        glm::vec2 offset = {0.0f, 0.0f};//!< Offset of the box collider

        /*!
         * \brief Draws the box collider shape using ImGui overlay.
         * \param entityPosition The world position of the entity.
         * \param transform The transform of the entity.
         */
        void ShowBounds(const glm::vec2& entityPosition, ECS::Transform transform) override {
            glm::vec2 center = glm::vec2(transform.pos) + offset;

            float angle = glm::eulerAngles(transform.rotation).z;

            glm::vec2 halfSize = size * 0.5f;

            glm::vec2 corners[4] = {
                    {-halfSize.x, -halfSize.y},
                    {halfSize.x, -halfSize.y},
                    {halfSize.x, halfSize.y},
                    {-halfSize.x, halfSize.y}
            };

            for (int i = 0; i < 4; ++i) {
                float x = corners[i].x, y = corners[i].y;
                corners[i].x = center.x + (x * cos(angle) - y * sin(angle));
                corners[i].y = center.y + (x * sin(angle) + y * cos(angle));
            }

            ImVec2 screenCorners[4];
            for (int i = 0; i < 4; ++i) { screenCorners[i] = EditorViews::WorldToScreenSpace(glm::vec3(corners[i], 0.0f)); }

            ImGui::GetWindowDrawList()->AddQuad(
                    screenCorners[0], screenCorners[1], screenCorners[2], screenCorners[3],
                    ImGui::GetColorU32(IM_COL32(0, 255, 0, 255)), 2.0f
                    );
        }
    };


    /*!
     * \struct CircleCollider2D
     * \brief Represents a circular 2D collider shape.
     */
    struct CircleCollider2D : public ShapeData2D {
        float radius = 1.0f;//!< Radius of the 2D circle collider
        glm::vec2 offset = {0.0f, 0.0f};//!< Offset of the circle collider

        /*!
         * \brief Draws the circle collider shape using ImGui overlay.
         * \param entityPosition The world position of the entity.
         * \param transform The transform of the entity.
         */
        void ShowBounds(const glm::vec2& entityPosition, ECS::Transform transform) override {
            glm::vec3 center3D = transform.pos + glm::vec3(offset, 0.0f);
            const int segments = 32;
            const float angleStep = glm::two_pi<float>() / segments;

            ImVec2 screenPoints[segments];

            for (int i = 0; i < segments; ++i) {
                float angle = i * angleStep;
                glm::vec2 localOffset = radius * glm::vec2(cos(angle), sin(angle));
                glm::vec3 worldPoint = center3D + glm::vec3(localOffset, 0.0f); // on XY plane
                screenPoints[i] = EditorViews::WorldToScreenSpace(worldPoint);
            }

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            for (int i = 0; i < segments; ++i) {
                drawList->AddLine(screenPoints[i], screenPoints[(i + 1) % segments], ImGui::GetColorU32(IM_COL32(0, 255, 255, 255)), 1.5f);
            }
        }

    };

    /*!
     * \struct Collider2D
     * \brief Wrapper that holds the type and shape data for a collider.
     *
     * Used to store collider metadata in a general way, allowing runtime polymorphism
     * and type-based handling (e.g., during collision resolution).
     */
    struct Collider2D {
        ColliderType2D type;//!< Type of the collider
        std::shared_ptr<ShapeData2D> shape;//!< Shared ptr to the shape data
    };
}

#endif //MARMALADE_COLLIDER2D_H