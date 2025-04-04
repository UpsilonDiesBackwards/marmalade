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
#include "../ecs/components/texturerenderer.h"
#include "mathematics/vector.h"

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
    std::string uuid;

    EntityFlags flags;
    Renderable renderable;
    Marmalade::ECS::ComponentManager componentManager{};

    std::weak_ptr<Entity> parent;
    std::vector<std::shared_ptr<Entity>> children;

    Entity(const std::string& name, const std::string& uuid, EntityFlags flags, bool withDefaultComponents = true);

    Entity(const std::string& name, EntityFlags flags, bool withDefaultComponents = true);

    Marmalade::Mathematics::Vec2 getPosition();
    void setPosition(Marmalade::Mathematics::Vec2 newPos);

    float getRotation();
    void setRotation(float newRot);

    Marmalade::Mathematics::Vec2 getScale();
    void setScale(Marmalade::Mathematics::Vec2 newScale);

    void UpdateModelMatrix();

    void AddChild(std::shared_ptr<Entity> parent, std::shared_ptr<Entity> child);
    void RemoveChild(Entity* child);

    bool HasParent() const;

    void Render();
};

#endif
