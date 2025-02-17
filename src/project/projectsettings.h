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

#ifndef MARMALADE_PROJECT_PROJECTSETTINGS_H
#define MARMALADE_PROJECT_PROJECTSETTINGS_H

#include <nlohmann/json.hpp>

#include <string>

namespace Marmalade::Project {
    struct ProjectSettings {
        std::string ProductName{"Marmalade"};
        std::string CompanyName{"Example Company"};
        std::string Description{"A marmalade project"};
        std::string Version{"v0.0.1"};
    };

    class Settings {
    public:
        static ProjectSettings projectSettings;

        static void SaveProjectSettings();
        static void LoadProjectSettings();
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectSettings, ProductName, CompanyName, Description, Version);
}

#endif
