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

        std::string description{};
        std::vector<std::string> dependencies{};
        std::vector<std::string> categories{};

        virtual void Display(Entity* entity) = 0;
        virtual void Apply(Entity* entity) = 0;
        virtual void Setup(Entity* entity) = 0;

        virtual nlohmann::json Serialize(const Entity* entity) = 0;
        virtual void Deserialize(nlohmann::json json, Entity* entity) = 0;

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

    struct RegisteredComponent {
        std::string Name;
        std::string Description;
        std::unique_ptr<IComponentFactory> Factory;
        std::vector<std::string> Dependencies{};
        std::vector<std::string> Categories{};
    };

    class ComponentRegistry {
    public:
        static ComponentRegistry& Instance() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void RegisterComponent(const std::string& name, const std::string& description, const std::vector<std::string> dependencies, const std::vector<std::string> categories) {
            _registry[name] = RegisteredComponent{
                    .Name = name,
                    .Description = description,
                    .Factory = std::make_unique<ComponentFactory<T>>(),
                    .Dependencies = std::move(dependencies),
                    .Categories = std::move(categories)};
        }

        [[nodiscard]] std::unique_ptr<Component> CreateComponent(const std::string& name, const std::string& uuid) {
            auto it = _registry.find(name);
            if (it != _registry.end()) {
                return it->second.Factory->Create(uuid);
            }
            return nullptr;
        }

        std::unordered_map<std::string, RegisteredComponent>& GetRegisteredComponents() {
            return _registry;
        }

        void BuildCategoryTree() {
            _categoryTree.clear();
            for (auto& [name, component]: _registry) {
                for (const std::string& category: component.Categories) {
                    _categoryTree[category].push_back(&component);
                }
            }
        }

        std::unordered_map<std::string, std::vector<RegisteredComponent*>>& GetCategoryTree() {
            return _categoryTree;
        }

        void SetFavourite(const std::string& name, bool favourite = true) {
            if (_registry.find(name) == _registry.end()) {
                // No component exists with that name
                return;
            }

            auto* component = &_registry[name];
            if (favourite) {
                _favourites.push_back(component);
            } else {
                _favourites.erase(std::remove(_favourites.begin(), _favourites.end(), component), _favourites.end());
            }
        }

        std::vector<RegisteredComponent*>& GetFavorites() {
            return _favourites;
        }

    private:
        std::unordered_map<std::string, RegisteredComponent> _registry;
        std::unordered_map<std::string, std::vector<RegisteredComponent*>> _categoryTree;
        std::vector<RegisteredComponent*> _favourites;
    };

#define REGISTER_COMPONENT(TYPE)                                                                                                        \
    static bool TYPE##_registered = [] {                                                                                                \
        ComponentRegistry::Instance().RegisterComponent<TYPE>(TYPE().name, TYPE().description, TYPE().dependencies, TYPE().categories); \
        return true;                                                                                                                    \
    }()
}

#endif
