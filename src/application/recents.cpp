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

#include "recents.h"

#include "config/configutil.h"

#include <fstream>

Marmalade::RecentsConfig Marmalade::Recents::recentsConfig{};

void Marmalade::Recents::LoadRecents() {
    std::ifstream i(ConfigUtil::GetConfigDirectory() / "recents.json");
    if (i.fail()) {
        // File doesn't exist
        recentsConfig = {};
        SaveRecents();
        return;
    }

    auto data = nlohmann::json::parse(i);
    recentsConfig = data.template get<RecentsConfig>();
    i.close();
}

void Marmalade::Recents::SaveRecents() {
    std::ofstream o(ConfigUtil::GetConfigDirectory() / "recents.json");
    nlohmann::json new_config = recentsConfig;
    o << new_config.dump(2);
    o.close();
}

void Marmalade::Recents::AddRecentProject(Marmalade::RecentProject project) {
    auto it = std::find(recentsConfig.projects.begin(), recentsConfig.projects.end(), project);

    // If project is already in recents, remove it
    if (it != recentsConfig.projects.end()) {
        recentsConfig.projects.erase(it);
    }

    // Add it to the top
    recentsConfig.projects.insert(recentsConfig.projects.begin(), project);
}
