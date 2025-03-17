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

#ifndef MARMALADE_PROJECT_PROJECTFILE_H
#define MARMALADE_PROJECT_PROJECTFILE_H

#include "../application/config/config.h"

#define PROJECT_FILE_VERSION 1

namespace Marmalade::Project {

    struct ProjectPaths {
        std::string settings{"settings.marm"};
        std::string packages{"package-settings.marm"};
        std::vector<std::string> scenes;
    };

    struct ProjectMarmalade {
        int version{PROJECT_FILE_VERSION};
        std::string type{"Marmalade::Project"};
        std::string name{"A Marmalade Project"};
        std::string uuid{};
        ProjectPaths paths{};
    };

    class ProjectFile : public Config<ProjectMarmalade> {
    public:
        explicit ProjectFile(const std::filesystem::path& filePath);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectPaths, settings, packages, scenes)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectMarmalade, version, type, name, uuid, paths)
}

#endif
