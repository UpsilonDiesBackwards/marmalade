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

#ifndef MARMALADE_PROJECT_PROJECT_H
#define MARMALADE_PROJECT_PROJECT_H

#include "projectfile.h"
#include "projectsettings.h"
#include "projectscenes.h"
#include "projectpackages.h"

#include "../gui/wizards/projectwizard.h"

#include <string>

namespace Marmalade::Project {

    class Project {
    public:
        std::string name;
        std::filesystem::path basePath; // Project base directory
        std::filesystem::path filePath; // Where the project is stored / project.marmalade file path

        ProjectSettings settings;
        std::shared_ptr<ProjectFile> projectMarmalade;
        ProjectPackages packages;

        ProjectScenes scenes;

        explicit Project(const std::filesystem::path& filePath);
        Project(std::string name, const std::filesystem::path& filePath);

        void CreateEmptyProject(ProjectCreationOptions creationOptions);

        void LoadProjectSettings();
        void SaveProjectSettings();

        void LoadProjectPackages();
        void SaveProjectPackages();

        bool CheckIfGitRepository();

    private:
        std::vector<std::string> baseDirectories = { // Directories auto-created when the project is made
                "assets",
                "windows",
                "logs"
        };

        std::vector<std::string> baseFiles = { // Files auto-created when the project is made
                "project.marmalade",
                "settings.marm",
                "package-settings.marm",
                "user.marm",
                ".assetreg",
                ".gitignore",
                "README.md",
        };
    };
}

#endif
