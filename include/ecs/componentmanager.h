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

#ifndef MARMALADE_ECS_COMPONENTMANAGER_H
#define MARMALADE_ECS_COMPONENTMANAGER_H

#include "component.h"

namespace Marmalade::ECS {
    class ComponentManager {
    public:
        std::vector<std::shared_ptr<Component>> components{};

        void AddComponent(std::shared_ptr<Component> component);

        void RemoveComponent(Component* component);

        template<typename T>
        T* GetComponentOfType() {
            for (auto& component: components) { if (auto derived = dynamic_cast<T*>(component.get())) { return derived; } }

            return nullptr;
        }

        template<typename T>
        const T* GetComponentOfType() const {
            for (auto& component: components) { if (auto derived = dynamic_cast<T*>(component.get())) { return derived; } }

            return nullptr;
        }

    };
}

#endif