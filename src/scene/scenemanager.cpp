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

#include "scenemanager.h"

#include <iostream>
#include "../application/logger.h"

SceneManager::SceneManager() : currentScene(nullptr) {
}

SceneManager::~SceneManager() {
    scenes.clear();
}

void SceneManager::AddScene(std::shared_ptr<Scene> scene) {
    const std::string& uuid = scene->GetUuid();
    const std::string& name = scene->GetName();

    if (scenes.find(uuid) != scenes.end()) { // check if uuid already exists
        return;
    }

    for (const auto& [existingUuid, existingScene] : scenes) { // check if name already exists
        if (existingScene->GetName() == name) {
            return;
        }
    }

    scenes[uuid] = scene;
}

void SceneManager::RemoveScene(std::shared_ptr<Scene> scene) {
    const std::string& uuid = scene->GetUuid();
    auto targetScene = scenes.find(uuid);
    if (targetScene != scenes.end()) {
        if (currentScene == targetScene->second) {
            currentScene = nullptr;
        }
        scenes.erase(targetScene);
    }
}

std::shared_ptr<Scene> SceneManager::GetCurrentScene() const {
    return currentScene;
}

std::string SceneManager::FindSceneUUIDByName(const std::string& sceneName, const std::string& sceneDir) {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(sceneDir)) {
        if (entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                continue;
            }

            try {
                nlohmann::json data;
                file >> data;

                if (data.contains("name")) {
                    std::string name = data["name"];

                    if (name == sceneName && data.contains("uuid")) {
                        std::string uuid = data["uuid"];
                        return uuid;
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Failed to parse: " << entry.path() << " (" << e.what() << ")" << std::endl;
            }
        }
    }

    return "";
}


void SceneManager::SetCurrentScene(const std::string &uuid) {
    auto scene = scenes.find(uuid);
    if (scene != scenes.end()) {
        if (currentScene) {
            for (auto entity : currentScene->GetEntities()) {
                auto* light = entity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();
                if (light) {
                    currentScene->RemoveLight(light);
                }
            }
        }

        currentScene = scene->second;

        for (auto entity : currentScene->GetEntities()) { // Add lights from the new scene
            auto* light = entity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();
            if (light) {
                currentScene->AddLight(light);
            }
        }
    }
}
