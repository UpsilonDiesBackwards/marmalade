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

#include <scene/scenemanager.h>

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

void SceneManager::SetCurrentScene(const std::string &uuid) {
    auto scene = scenes.find(uuid);
    if (scene != scenes.end()) {
        currentScene = scene->second;
    }
}
