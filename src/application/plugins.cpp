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

#include "plugins.h"

#include "config.h"

#include <fstream>

Marmalade::PluginsConfig Marmalade::Plugins::pluginsConfig{};

void Marmalade::Plugins::LoadPlugins() {
    std::ifstream i(Config::GetConfigDirectory() / "plugins.json");
    if (i.fail()) {
        // File doesn't exist
        pluginsConfig = {};
        SavePlugins();
        return;
    }

    auto data = nlohmann::json::parse(i);
    pluginsConfig = data.template get<PluginsConfig>();
    i.close();
}

void Marmalade::Plugins::SavePlugins() {
    std::ofstream o(Config::GetConfigDirectory() / "plugins.json");
    nlohmann::json new_config = pluginsConfig;
    o << new_config.dump(2);
    o.close();
}
