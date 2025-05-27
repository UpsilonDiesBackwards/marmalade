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
#include "../application/application.h"

#include <graphics/texture.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Entity::Entity(const std::string& name, const std::string& uuid, bool withDefaultComponents)
    : name(name), uuid(uuid), renderable(0, 0, 0, Texture::LoadTexture("", Marmalade::Material::TextureSettings{})) {

    renderable.Initialise();

    if (withDefaultComponents) {
        // Every entity should have a transform component by default
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Transform", Marmalade::Util::GenerateUUIDv4()));

        // Temporary
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Material Renderer", Marmalade::Util::GenerateUUIDv4()));
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Box Collider", Marmalade::Util::GenerateUUIDv4()));
        componentManager.AddComponent(Marmalade::ECS::ComponentRegistry::Instance().CreateComponent("Rigid Body", Marmalade::Util::GenerateUUIDv4()));
    }

    Render();
}

Entity::Entity(const std::string& name, bool withDefaultComponents) : Entity(name, Marmalade::Util::GenerateUUIDv4(), withDefaultComponents) {
}

void Entity::Render() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    if (!transform) return;

    for (const auto& component: componentManager.components) {
        component->Apply(this);
    }

    if (transform->isDirty) {
        UpdateModelMatrix();
    }

    bool hasTexture = componentManager.GetComponentOfType<Marmalade::ECS::MaterialRenderer>();

    renderable.Draw(this, transform->modelMatrix, hasTexture);

    renderable.ApplyLighting(Application::GetInstance().sceneManager.GetCurrentScene()->GetLights(),
                             glm::vec3(Application::GetInstance().camera->GetPosition(), 1.0f));

    for (auto& child: children) {
        child->Render();
    }
}

glm::vec3 Entity::getPosition() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->pos;
}

void Entity::setPosition(glm::vec3 newPos) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    transform->pos = newPos;
    transform->isDirty = true;
}

glm::vec3 Entity::getRotation() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return glm::degrees(glm::eulerAngles(transform->rotation));
}

void Entity::setRotation(glm::vec3 eulerDegrees) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    transform->rotation = glm::quat(glm::radians(eulerDegrees));
    transform->isDirty = true;
}

glm::vec3 Entity::getScale() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    return transform->scale;
}

void Entity::setScale(glm::vec3 newScale) {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    transform->scale = newScale;
    transform->isDirty = true;
}

void Entity::UpdateModelMatrix() {
    auto transform = componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    transform->modelMatrix = glm::mat4(1.0f);

    transform->modelMatrix = glm::translate(glm::mat4(1.0f), transform->pos)
                             * glm::mat4_cast(transform->rotation)
                             * glm::scale(glm::mat4(1.0f), transform->scale);

    if (auto parentPtr = parent.lock()) {
        const auto& parentMatrix = parentPtr->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->modelMatrix;
        transform->modelMatrix = parentMatrix * transform->modelMatrix;
    }

    for (auto& child : children) {
        child->UpdateModelMatrix();
    }

    transform->isDirty = false;
}


void Entity::AddChild(std::shared_ptr<Entity> parent, std::shared_ptr<Entity> child) {
    child->parent = parent;
    child->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()->isDirty = true;

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

bool Entity::HasParent() {
    return !parent.expired();
}

bool Entity::HasChild() {
    return !children.empty();
}

std::weak_ptr<Entity> Entity::GetParent() {
    return parent;
}
