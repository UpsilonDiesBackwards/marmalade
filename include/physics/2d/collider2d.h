
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
#include "../../../src/gui/editor/editor.h"

#include "../../../include/ecs/components/transform.h"

namespace Marmalade::Physics {
    enum class ColliderType2D {
        Box,
        Circle,
        OBB
    };

    struct ShapeData2D {
        virtual void ShowBounds(const glm::vec2& entityPosition, Marmalade::ECS::Transform transform) = 0;
        virtual ~ShapeData2D() = default;
    };

    struct BoxCollider2D : public ShapeData2D {
        glm::vec2 size = {1.0f, 1.0f};
        glm::vec2 offset = {0.0f, 0.0f};

        void ShowBounds(const glm::vec2 &entityPosition, ECS::Transform transform) override {
            glm::vec2 center = glm::vec2(transform.pos) + offset;

            float angle = glm::eulerAngles(transform.rotation).z;

            glm::vec2 halfSize = size * 0.5f;

            glm::vec2 corners[4] = {
                    {-halfSize.x, -halfSize.y},
                    { halfSize.x, -halfSize.y},
                    { halfSize.x,  halfSize.y},
                    {-halfSize.x,  halfSize.y}
            };

            for (int i = 0; i < 4; ++i) {
                float x = corners[i].x, y = corners[i].y;
                corners[i].x = center.x + (x * cos(angle) - y * sin(angle));
                corners[i].y = center.y + (x * sin(angle) + y * cos(angle));
            }

            ImVec2 screenCorners[4];
            for (int i = 0; i < 4; ++i) {
                screenCorners[i] = EditorViews::WorldToScreenSpace(corners[i]);
            }

            ImGui::GetWindowDrawList()->AddQuad(
                    screenCorners[0], screenCorners[1], screenCorners[2], screenCorners[3],
                    ImGui::GetColorU32(IM_COL32(0, 255, 0, 255)), 2.0f
            );
        }
    };

    struct CircleCollider2D : public ShapeData2D {
        float radius = 1.0f;
        glm::vec2 offset = {0.0f, 0.0f};

        void ShowBounds(const glm::vec2 &entityPosition, ECS::Transform transform) override {
            glm::vec2 center = glm::vec2(transform.pos) + offset;
            ImVec2 screenCenter = EditorViews::WorldToScreenSpace(center);

            ImVec2 screenEdge = EditorViews::WorldToScreenSpace(center + glm::vec2(radius, 0));
            float screenRadius = glm::distance(
                    glm::vec2(screenCenter.x, screenCenter.y),
                    glm::vec2(screenEdge.x, screenEdge.y)
            );

            ImGui::GetWindowDrawList()->AddCircle(
                    screenCenter, screenRadius,
                    ImGui::GetColorU32(IM_COL32(0, 255, 255, 255)), 32, 2.0f
            );
        }
    };

    struct Collider2D {
        ColliderType2D type;
        std::shared_ptr<ShapeData2D> shape;
    };
}

#endif //MARMALADE_COLLIDER2D_H
