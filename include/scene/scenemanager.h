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

#ifndef ENGINE_SCENEMANAGER_H
#define ENGINE_SCENEMANAGER_H

#include "scene.h"

#include <memory>
#include <unordered_map>

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::shared_ptr<Scene> scene);

    std::shared_ptr<Scene> GetSceneByUuid(const std::string& uuid) const;
    std::shared_ptr<Scene> GetCurrentScene() const;

    void SetCurrentScene(const std::string& uuid);
private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes; // UUID -> Scene
    std::shared_ptr<Scene> currentScene;
};

#endif
