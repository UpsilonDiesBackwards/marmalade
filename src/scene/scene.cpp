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

#include <scene/scene.h>

#include <iostream>
#include <algorithm>

Scene::Scene(const std::string& name) : name(name) {
}

Scene::~Scene() {
}

void Scene::AddEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::find(entities.begin(), entities.end(), entity));
}

void Scene::RemoveEntity(Entity* entity) {
    auto it = std::find_if(entities.begin(), entities.end(), [entity](const std::shared_ptr<Entity>& e) {
        return e.get() == entity;
    });

    if (it != entities.end()) {
        entities.erase(it);
    }
}

std::vector<std::shared_ptr<Entity>>& Scene::GetEntities() {
    return entities;
}

void Scene::Render() {
    for (const auto& entity: entities) {
        entity->Render();
    }
}

const std::string& Scene::GetName() const {
    return name;
}
