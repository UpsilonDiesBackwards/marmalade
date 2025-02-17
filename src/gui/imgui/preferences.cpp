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
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>

Marmalade::GUI::Preferences::Preferences() : Window() {
    _panes = {
            {"logging", PreferencesPane(drawGeneralLoggingPane)},
            {"appearance", PreferencesPane(drawGeneralAppearancePane)},
            {"projects", PreferencesPane(drawGeneralProjectsPane)}};
}

void Marmalade::GUI::Preferences::drawGeneralLoggingPane() {
    ImGui::Combo("Log Level", reinterpret_cast<int*>(&Config::engineConfig.LogLevel), getLogLevels, nullptr, spdlog::level::n_levels);
    ImGui::SameLine();
    requiresRestartWarning();
}

void Marmalade::GUI::Preferences::drawGeneralAppearancePane() {
    ImGui::Checkbox("ImGui Viewports", &Marmalade::Config::engineConfig.Viewports);
    ImGui::SameLine();
    requiresRestartWarning();

    static char themeFileC[512];
    strncpy(themeFileC, Config::engineConfig.ThemeFile.c_str(), sizeof(themeFileC));

    if (ImGui::InputText("Theme File", themeFileC, sizeof(themeFileC))) {
        Config::engineConfig.ThemeFile = themeFileC;
    }
    ImGui::SameLine();
    requiresRestartWarning();
}

void Marmalade::GUI::Preferences::drawGeneralProjectsPane() {
    static char defaultProjectPathC[512];
    strncpy(defaultProjectPathC, Config::engineConfig.DefaultProjectPath.c_str(), sizeof(defaultProjectPathC));

    if (ImGui::InputText("Default Project Path", defaultProjectPathC, sizeof(defaultProjectPathC))) {
        Config::engineConfig.DefaultProjectPath = defaultProjectPathC;
    }
}

void Marmalade::GUI::Preferences::selectableTreeNode(const char* title, const char* id) {
    if (ImGui::Selectable(title, _selectedItem == id)) {
        _selectedItem = id;
    }
}

void Marmalade::GUI::Preferences::drawLeftPane() {
    if (ImGui::TreeNode("General")) {
        selectableTreeNode("Logging", "logging");
        selectableTreeNode("Appearance", "appearance");
        selectableTreeNode("Projects", "projects");

        ImGui::TreePop();
    }
}

void Marmalade::GUI::Preferences::drawRightPane() {
    if (_panes.contains(_selectedItem)) {
        _panes[_selectedItem].DrawFunc();
    }

    // Position Save button at bottom right
    ImVec2 area = ImGui::GetWindowSize();
    ImGui::SetCursorPos(ImVec2(area.x - 60.0f, area.y - 30.0f));

    if (ImGui::Button("Save")) {
        Marmalade::Config::SaveEngineConfig();

        for (auto& pane: _panes) {
            if (pane.second.SaveFunc) {
                pane.second.SaveFunc();
            }
        }
    }
}

void Marmalade::GUI::Preferences::drawSplit() {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static float split_ratio = 0.3f;
    static float min_size = 50.0f;

    float left_width = area.x * split_ratio;
    float right_width = area.x - left_width - 4.0f;

    left_width = ImMax(left_width, min_size);
    right_width = ImMax(right_width, min_size);

    ImGui::BeginChild("PreferencesLeftPane", ImVec2(left_width, area.y), true);
    drawLeftPane();
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::InvisibleButton("##PreferencesSplitter", ImVec2(8.0f, area.y), ImGuiButtonFlags_None);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    if (ImGui::IsItemActive()) {
        ImGuiIO& io = ImGui::GetIO();
        split_ratio += io.MouseDelta.x / area.x;
        split_ratio = ImClamp(split_ratio, min_size / area.x, 1.0f - min_size / area.x);
    }

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("PreferencesRightPane", ImVec2(right_width, area.y), true);
    drawRightPane();
    ImGui::EndChild();
}

void Marmalade::GUI::Preferences::Draw() {
    ImGui::Begin(ICON_CI_SETTINGS_GEAR " Preferences", &visible);

    drawSplit();

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
