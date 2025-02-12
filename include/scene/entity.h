/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "../graphics/renderable.h"
#include "../ecs/componentmanager.h"
#include "../ecs/components/transform.h"
#include "../ecs/components/spriterender.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

enum EntityFlags {
    ACTIVE = 1 << 0,
    HAS_BOUNDS = 1 << 2,
    PLAYER_CONTROLLED = 1 << 3,
    RENDERABLE = 1 << 4,
    NONE,
};

struct Entity {
public:
    unsigned int id;

    std::string name;
    EntityFlags flags;
    Renderable renderable;
    Marmalade::ECS::ComponentManager componentManager{};

    // Temp
    std::shared_ptr<Marmalade::ECS::Transform> transform = std::make_shared<Marmalade::ECS::Transform>();
    std::shared_ptr<Marmalade::ECS::SpriteRender> spriteRender = std::make_shared<Marmalade::ECS::SpriteRender>();

    Entity* parent{nullptr};
    std::vector<std::unique_ptr<Entity>> children;

    Entity(const std::string& name, EntityFlags flags);

    glm::vec2 getPosition();
    void setPosition(glm::vec2 newPos);

    float getRotation();
    void setRotation(float newRot);

    glm::vec2 getScale();
    void setScale(glm::vec2 newScale);

    void UpdateModelMatrix();

    void AddChild(std::unique_ptr<Entity> child);
    void RemoveChild(Entity* child);

    bool HasParent() const;
    void SetParent(Entity* newParent);

    void Render();
};

#endif
