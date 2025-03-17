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

#include "config.h"

#include <nlohmann/json.hpp>

#include <vector>

#define RECENTS_VERSION 1

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
        int version{RECENTS_VERSION};
        std::vector<RecentProject> projects{};
    };

    class Recents : public Config<RecentsConfig> {
    public:
        explicit Recents();

        CONFIG_SINGLETON(Recents)

        CONFIG_DECL_GET_FUNC(Recents)

        void AddRecentProject(RecentProject project);
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::RecentProject, name, uuid, path);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::RecentsConfig, version, projects);
}


#endif
