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

#ifndef MARMALADE_ENGINECONFIG_H
#define MARMALADE_ENGINECONFIG_H

#include "config.h"

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

#define CONFIG_VERSION 1

namespace Marmalade {
    struct Repository {
        std::string name;
        std::string gitUrl{};
        int depth{1};// Local repos do not work with shallow clone
    };

    struct InterfaceConfig {
        std::string graphicsSystem{"opengl"};
        std::string graphicsVersion{"4.3"};
    };

    struct AppearanceConfig {
        bool viewports{true};
        std::string themeFile{"editorstyle.txt"};
        bool showWelcomeScreen{true};
        long long backgroundColor = 8589934591;
        float scaleFactor{1.0f};
        bool useSystemScaleFactor{true};
        std::string language{"en"};
    };

    struct WindowPosConfig {
        int monitor{0};
        int x{0};
        int y{0};
        int width{800};
        int height{600};
        bool maximised{true};
    };

    struct ProjectBrowserConfig {
        long long colorAssets = 4279242858;
        long long colorData = 4281109100;
        long long colorSrc = 4284895275;
    };

    struct EngineConfigStruct {
        int version{CONFIG_VERSION};

        InterfaceConfig interface;
        AppearanceConfig appearance;
        WindowPosConfig windowPos;

        spdlog::level::level_enum logLevel{spdlog::level::info};

        std::string defaultProjectPath{"."};
        std::vector<Repository> repos{};

        ProjectBrowserConfig projectBrowser{};

        std::vector<std::string> favouriteComponents{};

        std::string audioOutputDevice{""};
    };

    class EngineConfig : public Config<EngineConfigStruct> {
    public:
        explicit EngineConfig();

        CONFIG_SINGLETON(EngineConfig)

        CONFIG_DECL_GET_FUNC(EngineConfig)

        void PrepareNewConfig() override;

    private:
#pragma region Migrations

        static void migrateFromVersion1(nlohmann::json& data);

#pragma endregion
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Repository, name, gitUrl, depth);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::InterfaceConfig, graphicsSystem, graphicsVersion);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::AppearanceConfig, viewports, themeFile, showWelcomeScreen, backgroundColor, scaleFactor, useSystemScaleFactor, language);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::WindowPosConfig, monitor, x, y, width, height, maximised);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::ProjectBrowserConfig, colorAssets, colorData, colorSrc);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::EngineConfigStruct, version, interface, appearance, windowPos, logLevel, defaultProjectPath, repos, projectBrowser, favouriteComponents, audioOutputDevice);
}

#endif
