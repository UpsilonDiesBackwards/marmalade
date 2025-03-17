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

#include "engineconfig.h"

#include "configutil.h"

Marmalade::EngineConfig::EngineConfig() : Config<EngineConfigStruct>(ConfigUtil::GetConfigDirectory() / "settings.json") {
    version = CONFIG_VERSION;
    useGui = false;

    AddMigration(1, migrateFromVersion1);
}

void Marmalade::EngineConfig::PrepareNewConfig() {
    storedConfig.repos.push_back(Repository{"default", "https://github.com/UpsilonDiesBackwards/marmalade-pkgs.git", 1});
}

#pragma region Migrations

void Marmalade::EngineConfig::migrateFromVersion1(nlohmann::json& data) {
    // Stub here, when version is set to 2, add code here for adding extra keys
}

#pragma endregion
