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

#include "../application/util.h"
#include "../application/logger.h"

#include "mathematics/matrix.h"

#include <graphics/texture.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>

Entity::Entity(const std::string& name, const std::string& uuid, EntityFlags flags, bool withDefaultComponents)
    : name(name), uuid(uuid), flags(flags), renderable(0, 0, 0, Texture::LoadTexture("")) {

    renderable.Initialise();

    if (withDefaultComponents) {
        // Every entity should have a transform component by default
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Transform", Marmalade::Util::GenerateUUIDv4()));

        // Temporary
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Texture Renderer", Marmalade::Util::GenerateUUIDv4()));
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Box Collider", Marmalade::Util::GenerateUUIDv4()));
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Rigid Body", Marmalade::Util::GenerateUUIDv4()));
    }

    Render();
}

Entity::Entity(const std::string& name, EntityFlags flags, bool withDefaultComponents) : Entity(name, Marmalade::Util::GenerateUUIDv4(), flags, withDefaultComponents) {
}

void Entity::Render() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    if (!transform) return;

    for (const auto& component: componentManager.components) {
        component->Apply(this);
    }

    if (RENDERABLE) {
        bool hasTexture = componentManager.GetComponentOfType<Marmalade::ECS::TextureRenderer>();

        renderable.Draw(transform->modelMatrix, hasTexture);
    }

    for (auto& child: children) {
        child->Render();
    }
}

Marmalade::Mathematics::Vec2 Entity::getPosition() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->pos;
}

void Entity::setPosition(Marmalade::Mathematics::Vec2 newPos) {
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

Marmalade::Mathematics::Vec2 Entity::getScale() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->scale;
}

void Entity::setScale(Marmalade::Mathematics::Vec2 newScale) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    transform->scale = newScale;
    UpdateModelMatrix();
}

void Entity::UpdateModelMatrix() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    transform->modelMatrix = Marmalade::Mathematics::Mat4::Identity();

    auto translation = Marmalade::Mathematics::Mat4::Translate(
            Marmalade::Mathematics::Vec3(transform->pos[0], transform->pos[1], 0.0f)
    );

    auto rotation = Marmalade::Mathematics::Mat4::Rotate(
            transform->rotation,
            Marmalade::Mathematics::Vec3(0.0f, 0.0f, 1.0f)
    );

    auto scaling = Marmalade::Mathematics::Mat4::Scale(
            Marmalade::Mathematics::Vec3(transform->scale[0], transform->scale[1], 1.0f)
    );

    transform->modelMatrix = translation * rotation * scaling;

    if (auto parentPtr = parent.lock()) {
        transform->modelMatrix = parentPtr->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->modelMatrix * transform->modelMatrix;
    }

    for (auto& child: children) {
        child->UpdateModelMatrix();
    }
}

void Entity::AddChild(std::shared_ptr<Entity> parent, std::shared_ptr<Entity> child) {
    child->parent = parent;
    children.push_back(std::move(child));

    LOG_INFO("child: {}", this->children[0]->name);
}

void Entity::RemoveChild(Entity* target) {
    auto i = std::remove_if(children.begin(), children.end(),
                            [target](const std::shared_ptr<Entity>& child) {
                                return child.get() == target;
                            });

    if (i != children.end()) {
        children.erase(i, children.end());
    }
}

bool Entity::HasParent() const {
    return !parent.expired();
}
