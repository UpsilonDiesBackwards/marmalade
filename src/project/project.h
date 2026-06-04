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

#include "index.h"
#include "projectfile.h"
#include "projectscenes.h"

#include "../gui/wizards/projectwizard.h"
#include "settings/settingscontainer.h"

#include <string>

namespace Marmalade::Project {

    class Project {
    public:
        static const char* MARM_DIR;

        std::string name;
        std::filesystem::path basePath;// Project base directory
        std::filesystem::path filePath;// Where the project is stored / project.marmalade file path

        /**
         * Stores the deserialized contents of the project.marmalade file.
         */
        std::shared_ptr<ProjectFile> projectMarmalade;

        /**
         * Stores the deserialized contents of the settings files in the .marm directory. Each of these is known as a "settings object".
         */
        std::shared_ptr<SettingsContainer> projectSettings;

        std::shared_ptr<Index> projectIndex;

        ProjectScenes scenes;

        explicit Project(const std::filesystem::path& filePath);
        Project(std::string name, const std::filesystem::path& filePath);

        void CreateEmptyProject(ProjectCreationOptions creationOptions);

        void LoadProjectSettings(GUI::ConfigErrorDialog* errorDlg = nullptr);
        void SaveProjectSettings() const;

        void LoadProjectPackages();

        void InitDatabase();
        void CloseDatabase();

        bool CheckIfGitRepository() const;

    private:
        std::vector<std::string> baseDirectories = {// Directories auto-created when the project is made
                MARM_DIR,
                "assets",
                "data",
                "logs",
                "packages",
                "src"};

        template<typename T>
        bool initSettingsObject(SettingsObject<T>& object) {
            object.config.filePath = basePath / projectMarmalade->GetPathOrDefault(object.name, std::string(MARM_DIR) + "/" + object.name + ".marm");

            return true;
        }

        template<typename T>
        bool loadSettingsObject(SettingsObject<T>& object, GUI::ConfigErrorDialog* errorDlg) {
            initSettingsObject(object);
            object.config.useGui = true;
            object.config.LoadConfig(errorDlg);

            return true;
        }
    };
}

#endif
