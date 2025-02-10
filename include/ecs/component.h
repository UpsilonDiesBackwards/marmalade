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

#ifndef MARMALADE_COMPONENT_H
#define MARMALADE_COMPONENT_H

#include <spdlog/spdlog.h>

#include <typeindex>

class Entity;

class Component {
public:
    std::string name;

    virtual void Display(Entity* entity) = 0;
    virtual void Apply(Entity *entity) = 0;

    static std::unordered_map<std::type_index , std::string>& GetRegisteredComponents() {
        static std::unordered_map<std::type_index, std::string> registry;
        return registry;
    }

    static void RegisterComponent(std::type_index type, const std::string& name) {
        GetRegisteredComponents()[type] = name;
    }

    virtual ~Component() = default;
};

// THANKS JAY FOR THE HELP WITH MACROS
#define REGISTER_COMPONENT(TYPE) \
    static bool _registered; \
    static bool Register() { \
    Component::RegisterComponent(typeid(TYPE), #TYPE); \
    return true; \
    }

#endif
