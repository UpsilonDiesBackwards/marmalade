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

#ifndef MARMALADE_ECS_COMPONENT_H
#define MARMALADE_ECS_COMPONENT_H

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include <typeindex>
#include <functional>
#include <utility>

class Entity;

namespace Marmalade::ECS {
    class Component {
    public:
        std::string name;
        std::string uuid;

        bool isMutable{true};
        bool allowMultiple{false};

        std::vector<std::string> dependencies{};

        virtual void Display(Entity* entity) = 0;
        virtual void Apply(Entity* entity) = 0;

        virtual nlohmann::json Serialize() = 0;
        virtual void Deserialize(nlohmann::json json) = 0;

        virtual ~Component() = default;
    };

    class IComponentFactory {
    public:
        [[nodiscard]] virtual std::unique_ptr<Component> Create(const std::string& uuid) const = 0;
        virtual ~IComponentFactory() = default;
    };

    template<typename T>
    class ComponentFactory : public IComponentFactory {
    public:
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");

        [[nodiscard]] std::unique_ptr<Component> Create(const std::string& uuid) const override {
            auto component = std::make_unique<T>();
            component->uuid = uuid;
            return component;
        }
    };

    class ComponentRegistry {
    public:
        static ComponentRegistry& Instance() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void RegisterComponent(const std::string& name) {
            _registry[name] = std::make_unique<ComponentFactory<T>>();
        }

        [[nodiscard]] std::unique_ptr<Component> CreateComponent(const std::string& name, const std::string& uuid) {
            auto it = _registry.find(name);
            if (it != _registry.end()) {
                return it->second->Create(uuid);
            }
            return nullptr;
        }

        std::unordered_map<std::string, std::unique_ptr<IComponentFactory>>& GetRegisteredComponents() {
            return _registry;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<IComponentFactory>> _registry;
    };

#define REGISTER_COMPONENT(TYPE)                                            \
    static bool TYPE##_registered = [] {                                    \
        ComponentRegistry::Instance().RegisterComponent<TYPE>(TYPE().name); \
        return true;                                                        \
    }()

}

#endif
