/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "projectscenes.h"

#include "../application/application.h"

#include "project.h"

#include <fstream>

void Marmalade::Project::ProjectScenes::RegisterScene(const std::string& fileName) {
    auto project = Application::GetInstance().GetCurrentProject();
    auto& scenes = project->projectMarmalade.paths.scenes;

    if (std::find(scenes.begin(), scenes.end(), fileName) == scenes.end()) {
        scenes.push_back(fileName);
    }
}

void Marmalade::Project::ProjectScenes::SaveScene(const std::string& fileName, Scene* scene) {
    auto project = Application::GetInstance().GetCurrentProject();

    nlohmann::json j;
    j["name"] = scene->GetName();

    // TODO: Entities should be in own files
    j["entities"] = nlohmann::json::array();
    for (const auto& entity: scene->GetEntities()) {
        j["entities"].push_back(serializeEntity(entity.get()));
    }

    std::ofstream file(project->basePath / fileName);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

Scene Marmalade::Project::ProjectScenes::LoadScene(const std::string& fileName) {
    auto project = Application::GetInstance().GetCurrentProject();
    std::ifstream file(project->basePath / fileName);

    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        file.close();

        auto scene = Scene(j["name"]);

        for (const auto& entityJson: j["entities"]) {
            scene.AddEntity(std::make_shared<Entity>(deserializeEntity(entityJson)));
        }

        return scene;
    }

    return Scene("");
}

void Marmalade::Project::ProjectScenes::UnregisterScene(const std::string& fileName) {
    auto project = Application::GetInstance().GetCurrentProject();

    auto& scenes = project->projectMarmalade.paths.scenes;

    scenes.erase(std::remove_if(scenes.begin(), scenes.end(),
                                [&fileName](const std::string& filePath) {
                                    return filePath == fileName;
                                }),
                 scenes.end());
}

nlohmann::json Marmalade::Project::ProjectScenes::serializeEntity(const Entity* entity) {
    nlohmann::json e;
    e["name"] = entity->name;

    for (const auto& component: entity->componentManager.components) {
        e["components"].push_back(component->name);
        // TODO: Components need function to serialize and deserialize
    }

    for (const auto& child: entity->children) {
        e["entities"].push_back(serializeEntity(child.get()));
    }

    return e;
}

Entity Marmalade::Project::ProjectScenes::deserializeEntity(const nlohmann::json& e) {
    std::string name = e["name"];
    auto entity = Entity(name, EntityFlags::RENDERABLE);

    // Deserialize components
    for (const auto& componentName: e["components"]) {
        auto factory = Marmalade::ECS::ComponentRegistry::Instance().GetRegisteredComponents()[componentName].get();

        entity.componentManager.AddComponent(factory->Create()); // Add component
    }

    // Deserialize children (recursive)
    if (e.contains("entities")) {
        for (const auto& childEntityJson: e["entities"]) {
            entity.AddChild(std::make_unique<Entity>(deserializeEntity(childEntityJson)));
        }
    }

    return entity;
}

