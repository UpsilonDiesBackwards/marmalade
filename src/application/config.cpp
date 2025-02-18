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

#include "config.h"

#include <iostream>
#include <fstream>

#ifndef _WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#define APP_NAME "marmalade-engine"

Marmalade::EngineConfig Marmalade::Config::engineConfig{};

std::filesystem::path Marmalade::Config::_configDir{};

void Marmalade::Config::SetConfigDirectory(bool sameDirConfig) {
    if (sameDirConfig) {
        _configDir = std::filesystem::current_path();
        return;
    }

#ifdef _WIN32
    char* appData = std::getenv("APPDATA");
    if (appData) {
        _configDir = std::filesystem::path(appData) / APP_NAME;
    } else {
        _configDir = std::filesystem::path("C:/Users/Default/AppData/Roaming") / APP_NAME;
    }
#elif __APPLE__
    const char* home = std::getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    _configDir = std::filesystem::path(home) / "Library" / "Application Support" / APP_NAME;
#else
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfig) {
        _configDir = std::filesystem::path(xdgConfig) / APP_NAME;
    } else {
        const char* home = std::getenv("HOME");
        if (!home) home = getpwuid(getuid())->pw_dir;
        _configDir = std::filesystem::path(home) / ".config" / APP_NAME;
    }
#endif

    if (!std::filesystem::exists(_configDir)) {
        std::filesystem::create_directories(_configDir);
    }
}

std::filesystem::path Marmalade::Config::GetConfigDirectory() {
    return _configDir;
}

void Marmalade::Config::LoadEngineConfig() {
    std::ifstream i(_configDir / "settings.json");
    if (i.fail()) {
        // File doesn't exist
        engineConfig = {};
        engineConfig.repos.push_back(Repository{"default", "https://github.com/UpsilonDiesBackwards/marmalade-pkgs.git", 1});
        SaveEngineConfig();
        return;
    }

    auto data = nlohmann::json::parse(i);
    engineConfig = data.template get<EngineConfig>();
    i.close();
}

void Marmalade::Config::SaveEngineConfig() {
    std::ofstream o(_configDir / "settings.json");
    nlohmann::json new_config = engineConfig;
    o << new_config.dump(2);
    o.close();
}
