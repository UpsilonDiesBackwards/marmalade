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

#include <ecs/componentmanager.h>

#include "../application/util.h"
#include "../application/logger.h"

void Marmalade::ECS::ComponentManager::AddComponent(std::shared_ptr<Component> component) {
    if (component == nullptr) {
        return;
    }

    if (!component->allowMultiple) {
        for (const auto& existing: components) {
            if (typeid(*component) == typeid(*existing)) {
                LOG_ERROR("Can not add {}, component already exists", component->name);
                return;
            }
        }
    }

    for (const auto& dependency: component->dependencies) {// Loop through each dependency...
        bool hasDependency = false;
        for (const auto& existing: components) {
            if (existing->name == dependency) {
                hasDependency = true;
                break;
            }
        }

        if (!hasDependency) {// ... and add any if it's not already added
            auto newDependency = Marmalade::ECS::ComponentRegistry::Instance().CreateComponent(dependency, Util::GenerateUUIDv4());
            if (newDependency) {
                components.push_back(std::move(newDependency));
                LOG_WARN("Automatically add {} component as {} depends on it", dependency, component->name);
            }
        }
    }

    components.push_back(component);// if a component has no dependencies, add it immediately
}

void Marmalade::ECS::ComponentManager::RemoveComponent(Component* component) {
    for (const auto& existing: components) {// Check for dependencies before removing a component
        for (const auto& dependency: existing->dependencies) {
            if (component->name == dependency) {
                LOG_ERROR("Could not remove {} because its a dependency of {}", component->name, existing->name);
                return;
            }
        }
    }

    components.erase(
            std::remove_if(components.begin(), components.end(),
                           [component](const std::shared_ptr<Component>& comp) {
                               return comp.get() == component;
                           }),
            components.end());
}
