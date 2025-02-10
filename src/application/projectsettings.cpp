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

#include "projectsettings.h"

#include "application.h"

#include <fstream>

ProjectSettings Settings::projectSettings;

void Settings::SaveProjectSettings() {
    Application& app = Application::GetInstance();
    auto project = app.GetCurrentProject();

    std::ofstream o(std::filesystem::path(project->filePath) / "settings.marm");
    nlohmann::json new_settings = projectSettings;
    o << new_settings.dump(2);
    o.close();
}

void Settings::LoadProjectSettings() {
    Application& app = Application::GetInstance();
    auto project = app.GetCurrentProject();

    std::ifstream i(std::filesystem::path(project->filePath) / "settings.marm");
    if (i.fail()) {
        // File doesn't exist!
        spdlog::error("Failed to load project settings, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    projectSettings = data.template get<ProjectSettings>();
    i.close();
}
