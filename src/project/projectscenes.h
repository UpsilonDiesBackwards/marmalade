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

#ifndef MARMALADE_PROJECT_PROJECTSCENES_H
#define MARMALADE_PROJECT_PROJECTSCENES_H

#include <scene/scene.h>

#include <nlohmann/json.hpp>

#include <string>

namespace Marmalade::Project {
    class Project;

    class ProjectScenes {
    public:
        struct Component {
            std::string name;
            std::string uuid;
            nlohmann::json data;
        };

        static std::string GetSceneFileName(const std::string &sceneName);

        void RegisterScene(const std::string& fileName);

        void SaveScene(const std::string& fileName, Scene* scene);

        Scene LoadScene(const std::string& fileName);

        void UnregisterScene(const std::string& fileName);

    private:
        nlohmann::json serializeEntity(const Entity* entity);
        Entity deserializeEntity(const nlohmann::json& e);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectScenes::Component, name, uuid, data);
}


#endif
