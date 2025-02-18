/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_CONFIG_H
#define MARMALADE_CONFIG_H

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

#include <filesystem>

namespace Marmalade {

    struct Repository {
        std::string name;
        std::string gitUrl{};
        int depth{1};// Local repos do not work with shallow clone
    };

    struct EngineConfig {
        bool viewports{true};
        std::string themeFile{"editorstyle.txt"};

        spdlog::level::level_enum logLevel{spdlog::level::info};

        std::string defaultProjectPath{"."};
        std::vector<Repository> repos{};
    };

    class Config {
    public:
        static EngineConfig engineConfig;

        static void SetConfigDirectory(bool sameDirConfig);
        static std::filesystem::path GetConfigDirectory();

        static void LoadEngineConfig();
        static void SaveEngineConfig();

    private:
        static std::filesystem::path _configDir;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Repository, name, gitUrl, depth);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::EngineConfig, viewports, themeFile, logLevel, defaultProjectPath, repos);
}

#endif
