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
        std::string type{"Marmalade::Project::Settings"};

        // Project
        std::string productName{"Marmalade Project"};
        std::string companyName{"Example Company"};
        std::string description{"A Marmalade Project"};
        std::string version{"v0.0.1"};

        // Graphics
        bool msaaEnabled = true;
        int msaaSampleCount = 8;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectSettings,
                                       type, productName, companyName, description, version,
                                       msaaEnabled, msaaSampleCount)
}

#endif
