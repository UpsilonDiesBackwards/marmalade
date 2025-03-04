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

#ifndef MARMALADE_RECENTS_H
#define MARMALADE_RECENTS_H

#include <nlohmann/json.hpp>

#include <vector>

namespace Marmalade {

    struct RecentProject {
        std::string name{};
        std::string uuid{};
        std::string path{};

        bool operator==(const RecentProject& other) const {
            return uuid == other.uuid;
        }
    };

    struct RecentsConfig {
        std::vector<RecentProject> projects{};
    };

    class Recents {
    public:
        static RecentsConfig recentsConfig;

        static void LoadRecents();
        static void SaveRecents();

        static void AddRecentProject(RecentProject project);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::RecentProject, name, uuid, path);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::RecentsConfig, projects);
}


#endif
