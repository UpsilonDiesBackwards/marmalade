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

#include "preferences.h"

#include "../../application/config/engineconfig.h"
#include "../../application/config/configutil.h"
#include "../../application/application.h"
#include "../../application/plugins/pluginloader.h"
#include "../../application/plugins/interfaceimpl.h"
#include "../../application/integration.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>

Marmalade::GUI::Preferences::Preferences() : Window() {
    _panes = {
            {"logging", PreferencesPane(drawGeneralLoggingPane)},
            {"appearance", PreferencesPane(drawGeneralAppearancePane)},
            {"projects", PreferencesPane(drawGeneralProjectsPane)},
            {"projectBrowser", PreferencesPane(drawGeneralProjectBrowserPane)},
            {"systemIntegration", PreferencesPane(drawGeneralSystemIntegrationPane)},
            {"plugins", PreferencesPane(drawGeneralPluginsPane)},
            {"input/output", PreferencesPane(drawAudioInputOutputPane)}};
}

void Marmalade::GUI::Preferences::drawGeneralLoggingPane() {
    ImGui::Combo("Log Level", reinterpret_cast<int*>(&EngineConfig::GetStoredConfig().logLevel), getLogLevels, nullptr, spdlog::level::n_levels);
    ImGui::SameLine();
    requiresRestartWarning();
}

void Marmalade::GUI::Preferences::drawGeneralAppearancePane() {
    ImGui::Checkbox("ImGui Viewports", &EngineConfig::GetStoredConfig().appearance.viewports);
    ImGui::SameLine();
    requiresRestartWarning();

    static char themeFileC[512];
    strncpy(themeFileC, EngineConfig::GetStoredConfig().appearance.themeFile.c_str(), sizeof(themeFileC));

    if (ImGui::InputText("Theme File", themeFileC, sizeof(themeFileC))) {
        EngineConfig::GetStoredConfig().appearance.themeFile = themeFileC;
    }
    ImGui::SameLine();
    requiresRestartWarning();

    static auto backgroundCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().appearance.backgroundColor);
    if (ImGui::ColorEdit4("Background Color", &backgroundCol.x)) {
        EngineConfig::GetStoredConfig().appearance.backgroundColor = ImGui::ColorConvertFloat4ToU32(backgroundCol);
    }

    ImGui::BeginDisabled(EngineConfig::GetStoredConfig().appearance.useSystemScaleFactor);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::DragFloat("Scale Factor", &EngineConfig::GetStoredConfig().appearance.scaleFactor, 0.1f, 0.0f, 5.0f);
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Checkbox("Use System", &EngineConfig::GetStoredConfig().appearance.useSystemScaleFactor);

    ImGui::SameLine();
    requiresRestartWarning();

    static char languageC[16];
    strncpy(languageC, EngineConfig::GetStoredConfig().appearance.language.c_str(), sizeof(themeFileC));

    if (ImGui::InputText("Language", languageC, sizeof(languageC))) {
        EngineConfig::GetStoredConfig().appearance.language = languageC;
    }
    ImGui::SameLine();
    requiresRestartWarning();

    ImGui::Checkbox("Automatic Workspace Switching", &EngineConfig::GetStoredConfig().appearance.automaticWorkspaceSwitching);
    ImGui::SetItemTooltip("Automatically switch to a different workspace topology when a monitor is connected or disconnected.");
}

void Marmalade::GUI::Preferences::drawGeneralProjectsPane() {
    static char defaultProjectPathC[512];
    strncpy(defaultProjectPathC, EngineConfig::GetStoredConfig().defaultProjectPath.c_str(), sizeof(defaultProjectPathC));

    if (ImGui::InputText("Default Project Path", defaultProjectPathC, sizeof(defaultProjectPathC))) {
        EngineConfig::GetStoredConfig().defaultProjectPath = defaultProjectPathC;
    }

    ImGui::Checkbox("Show Welcome Screen on Startup", &Marmalade::EngineConfig::GetStoredConfig().appearance.showWelcomeScreen);
}

void Marmalade::GUI::Preferences::drawGeneralSystemIntegrationPane() {
    using it = Application::Integration::IntegrationType;

    static bool fileAssoc = true;
    static bool launcher = true;

    unsigned int type = it::IntegrationType_NONE;

    if (fileAssoc)
        type |= it::IntegrationType_FILE_ASSOCIATION;
    if (launcher)
        type |= it::IntegrationType_LAUNCHER;

    ImGui::Checkbox("File Associations", &fileAssoc);
    ImGui::Checkbox("Launcher", &launcher);

    if (ImGui::Button("Add Integrations")) {
        Application::Integration::AddSystemIntegrations(static_cast<it>(type));
    }
}

void Marmalade::GUI::Preferences::drawGeneralPluginsPane() {
    auto loadedPlugins = PluginLoader::GetInstance().GetLoadedPlugins();

    ImGui::Text("%zu plugins loaded", loadedPlugins.size());

    static int selected = -1;
    auto pluginsDir = ConfigUtil::GetConfigDirectory() / "plugins";
    if (ImGui::BeginTable("##Plugins", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("File Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < loadedPlugins.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            auto relativePath = std::filesystem::relative(loadedPlugins[i].Path, pluginsDir).string();
            const bool isSelected = (selected == (int)i);
            if (ImGui::Selectable(relativePath.c_str(), isSelected)) selected = (int)i;

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(loadedPlugins[i].Type == PluginType_WASM ? "WASM" : "Native");
        }
        ImGui::EndTable();
    }
}

void Marmalade::GUI::Preferences::selectableTreeNode(const char* title, const char* id) {
    if (ImGui::Selectable(title, _selectedItem == id)) {
        _selectedItem = id;
    }
}

void Marmalade::GUI::Preferences::drawLeftPane() {
    if (ImGui::TreeNodeEx("General", ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode("Logging", "logging");
        selectableTreeNode("Appearance", "appearance");
        selectableTreeNode("Projects", "projects");
        selectableTreeNode("Project Browser", "projectBrowser");
        selectableTreeNode("System Integration", "systemIntegration");
        selectableTreeNode("Loaded Plugins", "plugins");

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Audio", ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode("Input/Output", "input/output");

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
        Marmalade::EngineConfig::GetInstance().SaveConfig();

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
    ImGui::SetNextWindowSize(ImVec2(800, 500));

    WINDOW_BEGIN(ICON_CI_SETTINGS_GEAR " Preferences", ImGuiWindowFlags_None)

    drawSplit();

    WINDOW_END()
}

void Marmalade::GUI::Preferences::drawGeneralProjectBrowserPane() {
    static auto assetsCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorAssets);
    static auto dataCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorData);
    static auto srcCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorSrc);

    if (ImGui::ColorEdit4("Assets", &assetsCol.x)) {
        EngineConfig::GetStoredConfig().projectBrowser.colorAssets = ImGui::ColorConvertFloat4ToU32(assetsCol);
    }

    if (ImGui::ColorEdit4("Data", &dataCol.x)) {
        EngineConfig::GetStoredConfig().projectBrowser.colorData = ImGui::ColorConvertFloat4ToU32(dataCol);
    }

    if (ImGui::ColorEdit4("Src", &srcCol.x)) {
        EngineConfig::GetStoredConfig().projectBrowser.colorSrc = ImGui::ColorConvertFloat4ToU32(srcCol);
    }
}

void Marmalade::GUI::Preferences::drawAudioInputOutputPane() {
    auto& engineConfig = EngineConfig::GetStoredConfig();
    auto& audioManager = ::Application::GetInstance().audioManager->GetInstance();

    std::vector<std::string> deviceList = audioManager.GetAvailableDevices();

    std::vector<const char*> cstrings;
    for (const auto& str : deviceList) {
        cstrings.push_back(str.c_str());
    }

    static int selectedDeviceIndex = 0;
    for (size_t i = 0; i < deviceList.size(); ++i) {
        if (deviceList[i] == engineConfig.audioOutputDevice) {
            selectedDeviceIndex = static_cast<int>(i);
            break;
        }
    }

    if (ImGui::Combo("Output device", &selectedDeviceIndex, cstrings.data(), cstrings.size())) {
        const std::string& selectedDevice = deviceList[selectedDeviceIndex];

        audioManager.SetCurrentDevice(selectedDevice.c_str());
        engineConfig.audioOutputDevice = selectedDevice;

        LOG_INFO("Selected device: {}", selectedDevice);
    }
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
