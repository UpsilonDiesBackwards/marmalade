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

std::string Marmalade::Project::ProjectScenes::GetSceneFileName(const std::string& sceneUuid) {
    return "data/" + sceneUuid + ".json";
}

std::filesystem::path Marmalade::Project::ProjectScenes::GetEntityDirectory() {
    auto project = Application::GetInstance().GetCurrentProject();
    auto dir = std::filesystem::path(project->basePath) / "data" / "entities";
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    return dir;
}

void Marmalade::Project::ProjectScenes::RegisterScene(const std::string& fileName) {
    // TODO: Add to scene index
}

void Marmalade::Project::ProjectScenes::SaveScene(const std::string& fileName, Scene* scene) {
    auto project = Application::GetInstance().GetCurrentProject();

    nlohmann::json j;
    j["type"] = "Marmalade::Scene";
    j["name"] = scene->GetName();
    j["uuid"] = scene->GetUuid();

    // TODO: Entities should be in own files
    j["entities"] = nlohmann::json::array();
    for (const auto& entity: scene->GetEntities()) {
        std::string entityFileName = entity->uuid + ".json";
        j["entities"].push_back(entityFileName);

        std::ofstream entityFile(GetEntityDirectory() / entityFileName);
        if (entityFile.is_open()) {
            entityFile << serializeEntity(entity.get()).dump(4);
            entityFile.close();
        }
    }

    std::ofstream file(project->basePath / fileName);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

Scene Marmalade::Project::ProjectScenes::LoadScene(const std::string& fileName, bool infoOnly) {
    auto project = Application::GetInstance().GetCurrentProject();
    std::ifstream file(project->basePath / fileName);

    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        file.close();

        if (j["type"] != "Marmalade::Scene") throw std::runtime_error("File is not a scene");

        auto scene = Scene(j["name"], j["uuid"]);
        if (infoOnly) return scene;

        for (const auto& entityFileName: j["entities"]) {
            std::ifstream entityFile(GetEntityDirectory() / entityFileName.get<std::string>());
            if (entityFile.is_open()) {
                nlohmann::json entityJson;
                entityFile >> entityJson;
                entityFile.close();

                scene.AddEntity(deserializeEntity(entityJson));
            }
        }

        return scene;
    }

    return Scene("", "");
}

void Marmalade::Project::ProjectScenes::UnregisterScene(const std::string& fileName) {
    // TODO: Remove from scene index
}

nlohmann::json Marmalade::Project::ProjectScenes::serializeEntity(const Entity* entity) {
    nlohmann::json e;
    e["type"] = "Marmalade::Entity";

    e["name"] = entity->name;
    e["uuid"] = entity->uuid;

    for (const auto& component: entity->componentManager.components) {
        auto componentData = component->Serialize(entity);
        if (componentData.is_null()) componentData = nlohmann::json::object();
        e["components"].push_back(Component{component->name, component->uuid, componentData});
    }

    e["entities"] = nlohmann::json::array();
    for (const auto& child: entity->children) {
        std::string entityFileName = child->uuid + ".json";
        e["entities"].push_back(entityFileName);

        std::ofstream entityFile(GetEntityDirectory() / entityFileName);
        if (entityFile.is_open()) {
            entityFile << serializeEntity(child.get()).dump(4);
            entityFile.close();
        }
    }

    return e;
}

std::shared_ptr<Entity> Marmalade::Project::ProjectScenes::deserializeEntity(const nlohmann::json& e) {
    std::string name = e["name"];
    std::string uuid = e["uuid"];
    auto entity = std::make_shared<Entity>(name, uuid, false);

    // Deserialize components
    for (const auto& componentJson: e["components"]) {
        auto component = componentJson.get<Component>();
        auto factory = Marmalade::ECS::ComponentRegistry::Instance().GetRegisteredComponents()[component.name].Factory.get();

        auto newComponent = factory->Create(component.uuid);
        newComponent->Deserialize(component.data, entity.get());
        entity->componentManager.AddComponent(std::move(newComponent));// Add component
    }

    for (const auto& component: entity->componentManager.components) {
        component->Setup(entity.get());
    }

    // Deserialize children (recursive)
    if (e.contains("entities")) {
        for (const auto& entityFileName: e["entities"]) {
            std::ifstream entityFile(GetEntityDirectory() / entityFileName);
            if (entityFile.is_open()) {
                nlohmann::json childJson;
                entityFile >> childJson;
                entityFile.close();

                entity->AddChild(entity, deserializeEntity(childJson));
            }
        }
    }

    return entity;
}

std::vector<Scene> Marmalade::Project::ProjectScenes::GetScenes() {
    auto project = Application::GetInstance().GetCurrentProject();

    std::vector<Index::Scene> indexScenes = project->projectIndex->GetScenesRows();
    std::vector<Scene> scenes{};
    scenes.reserve(indexScenes.size());
    for (const auto [uuid, name]: indexScenes) {
        scenes.emplace_back(name, uuid);
    }

    return scenes;
}
