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

#include <scene/entity.h>

#include <graphics/texture.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>

#include <spdlog/spdlog.h>

Entity::Entity(const std::string &name, EntityFlags flags)
    : name(name), flags(flags), renderable(0,0,0, Texture::LoadTexture("")) {

    renderable.Initialise();

    // Every entity should have a transform component by default
    componentManager.AddComponent(std::make_shared<Marmalade::ECS::Transform>());

    Render();
}

void Entity::Render() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    if (!transform) return;

    for (const auto &component : componentManager.components) {
        component->Apply(this);
    }

    if (RENDERABLE) {
        bool hasTexture = componentManager.GetComponentOfType<Marmalade::ECS::SpriteRender>();

        renderable.Draw(transform->modelMatrix, hasTexture);
    }

    for (auto &child : children) {
        child->Render();
    }
}

glm::vec2 Entity::getPosition() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->pos;
}

void Entity::setPosition(glm::vec2 newPos) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    transform->pos = newPos;
    UpdateModelMatrix();
}

float Entity::getRotation() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->rotation;
}

void Entity::setRotation(float newRot) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    transform->rotation = newRot;
    UpdateModelMatrix();
}

glm::vec2 Entity::getScale() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->scale;
}

void Entity::setScale(glm::vec2 newScale) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    transform->scale = newScale;
    UpdateModelMatrix();
}

void Entity::UpdateModelMatrix() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    transform->modelMatrix = glm::mat4(1.0f);

    transform->modelMatrix = glm::translate(transform->modelMatrix, glm::vec3(transform->pos, 0.0f));
    transform->modelMatrix = glm::rotate(transform->modelMatrix, glm::radians(transform->rotation),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
    transform->modelMatrix = glm::scale(transform->modelMatrix, glm::vec3(transform->scale, 1.0f));

    if (parent) {
        transform->modelMatrix = parent->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->modelMatrix * transform->modelMatrix;
    }

    for (auto& child : children) {
        child->UpdateModelMatrix();
    }
}

void Entity::AddChild(std::unique_ptr<Entity> child) {
    child->parent = this;
    children.push_back(std::move(child));

    spdlog::info("child: {}", this->children[0]->name);
}

void Entity::RemoveChild(Entity* target) {
    auto i = std::remove_if(children.begin(), children.end(),
                            [target](const std::unique_ptr<Entity>& child) {
                                return child.get() == target;
                            });

    if (i != children.end()) {
        children.erase(i, children.end());
    }
}

bool Entity::HasParent() const {
    return parent != nullptr;
}

void Entity::SetParent(Entity* newParent) {
    parent = newParent;
}
