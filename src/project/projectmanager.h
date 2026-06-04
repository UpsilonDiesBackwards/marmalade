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

#ifndef MARMALADE_PROJECT_PROJECTMANAGER_H
#define MARMALADE_PROJECT_PROJECTMANAGER_H

#include "project.h"
#include "../gui/windowmanager.h"

namespace Marmalade::Project {
    template<typename P = Project>
    class ProjectManager {
    public:
        static P CreateProject(std::string name, const std::filesystem::path& filePath, const ProjectCreationOptions& creationOptions) {
            P project(name, filePath);
            project.CreateEmptyProject(creationOptions);
            project.SaveProjectSettings();
            project.CloseDatabase();
            return project;
        }

        static P OpenProject(const std::filesystem::path& filePath) {
            P project(filePath);
            project.projectMarmalade->LoadConfig(&GUI::WindowManager::GetInstance().configErrorDlg);
            project.LoadProjectSettings(&GUI::WindowManager::GetInstance().configErrorDlg);
            project.LoadProjectPackages();
            project.InitDatabase();
            return project;
        }

        static void SaveProject(P* project) {
            project->projectMarmalade->SaveConfig();
            project->SaveProjectSettings();
        }
    };
}


#endif
