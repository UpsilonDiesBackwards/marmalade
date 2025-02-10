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

#include "preferences.h"

#include "../../application/config.h"

#include <imgui.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>

void Marmalade::GUI::Preferences::Draw() {
    static char defaultProjectPathC[512];
    strncpy(defaultProjectPathC, Config::engineConfig.defaultProjectPath.c_str(), sizeof(defaultProjectPathC));

    ImGui::Begin(ICON_CI_SETTINGS_GEAR " Preferences", &visible);

    ImGui::Checkbox("ImGui Viewports", &Marmalade::Config::engineConfig.Viewports);
    ImGui::SameLine();
    requiresRestartWarning();

    ImGui::Combo("Log Level", reinterpret_cast<int*>(&Config::engineConfig.LogLevel), getLogLevels, nullptr, spdlog::level::n_levels);
    ImGui::SameLine();
    requiresRestartWarning();

    if (ImGui::InputText("Default Project Path", defaultProjectPathC, sizeof(defaultProjectPathC))) {
        Config::engineConfig.defaultProjectPath = defaultProjectPathC;
    }

    if (ImGui::Button("Save")) {
        Marmalade::Config::SaveEngineConfig();
    }

    ImGui::End();
}

void Marmalade::GUI::Preferences::requiresRestartWarning() {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ICON_CI_WARNING " Requires restart");
}

bool Marmalade::GUI::Preferences::getLogLevels(void* data, int idx, const char** outText) {
    static const char* levels[] = {"Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off"};
    if (idx < 0 || idx >= spdlog::level::n_levels) return false;
    *outText = levels[idx];
    return true;
}
