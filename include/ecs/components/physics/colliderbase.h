
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

#ifndef MARMALADE_COLLIDERBASE_H
#define MARMALADE_COLLIDERBASE_H

#include "ecs/component.h"

#include "../../../../src/application/application.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Marmalade::ECS {
    struct AABBData {
        glm::vec2 size;
        glm::vec2 offset;
    };

    struct OBBData {
        glm::vec2 size;
        glm::vec2 offset;
        float rotation;
    };

    class ColliderBase : public Component {
    public:
        std::variant<AABBData, OBBData> data;

        bool showingBounds{false};

        virtual void Intersects(Entity* self, Entity* other) = 0;

        virtual bool IntersectsAABB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) = 0;
        virtual bool IntersectsOBB(const ColliderBase& other, const glm::vec2& posA, const glm::vec2& posB) = 0;

        template<typename T>
        T* GetCollisionData() {
            return std::get_if<T>(&data);
        }

        virtual void ShowBounds(const glm::vec2& entityPosition, Transform transform) = 0;

        ImVec2 WorldToScreenSpace(const glm::vec2& world) {
            Application& app = Application::GetInstance();

            glm::vec2 framebufferSize = glm::vec2(app.framebuffer->width, app.framebuffer->height);
            glm::vec2 framebufferPos = glm::vec2(app.framebuffer->position.x, app.framebuffer->position.y);

            glm::vec4 worldPos = glm::vec4(world, 0.0f, 1.0f);

            glm::vec4 clipSpace = app.camera->GetProjection() * app.camera->GetView() * worldPos;

            if (clipSpace.w != 0.0f) {
                clipSpace /= clipSpace.w;
            }

            float screenX = (clipSpace.x * 0.5f + 0.5f) * framebufferSize.x;
            float screenY = (1.0f - (clipSpace.y * 0.5f + 0.5f)) * framebufferSize.y; // Flip Y

            return ImVec2(screenX + framebufferPos.x, screenY + framebufferPos.y);
        }

    };
}

#endif//MARMALADE_COLLIDERBASE_H
