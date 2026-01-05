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

#include "../windowmanager.h"
#include "../../application/config/engineconfig.h"
#include "../../application/config/configutil.h"
#include "../../application/application.h"
#include "../../application/plugins/pluginloader.h"
#include "../../application/plugins/interfaceimpl.h"
#include "../../application/integration.h"

#include "../components/form.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>

Marmalade::GUI::Preferences::Preferences() : Window() {
    _panes = {
            {"logging", PreferencesPane(drawGeneralLoggingPane)},
            {"projects", PreferencesPane(drawGeneralProjectsPane)},
            {"systemIntegration", PreferencesPane(drawGeneralSystemIntegrationPane)},
            {"appearance", PreferencesPane(drawUIAppearancePane)},
            {"workspaces", PreferencesPane(drawUIWorkspacesPane)},
            {"projectBrowser", PreferencesPane(drawUIProjectBrowserPane)},
            {"localization", PreferencesPane(drawUILocalizationPane)},
            {"plugins", PreferencesPane(drawGeneralPluginsPane)},
            {"input/output", PreferencesPane(drawAudioInputOutputPane)},
            {"graphics", PreferencesPane(drawAdvancedGraphicsPane)}};
}

void Marmalade::GUI::Preferences::Draw() {
    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);

    WINDOW_BEGIN(ICON_WITH_TEXT(ICON_CI_SETTINGS_GEAR, _("Preferences")), ImGuiWindowFlags_NoDocking)

    drawSplit();

    WINDOW_END()
}

void Marmalade::GUI::Preferences::selectableTreeNode(const char* title, const char* id) {
    if (ImGui::Selectable(title, _selectedItem == id)) {
        _selectedItem = id;
    }
}

void Marmalade::GUI::Preferences::drawLeftPane() {
    if (ImGui::TreeNodeEx(pgettext("Preferences|", "General"), ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode(pgettext("Preferences|General|", "Logging"), "logging");
        selectableTreeNode(pgettext("Preferences|General|", "Projects"), "projects");
        selectableTreeNode(pgettext("Preferences|General|", "System Integration"), "systemIntegration");
        selectableTreeNode(pgettext("Preferences|General|", "Loaded Plugins"), "plugins");

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx(pgettext("Preferences|", "User Interface"), ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode(pgettext("Preferences|User Interface|", "Appearance"), "appearance");
        selectableTreeNode(pgettext("Preferences|User Interface|", "Workspaces"), "workspaces");
        selectableTreeNode(pgettext("Preferences|User Interface|", "Project Browser"), "projectBrowser");
        selectableTreeNode(pgettext("Preferences|User Interface|", "Localization"), "localization");

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx(pgettext("Preferences|", "Audio"), ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode(pgettext("Preferences|Input/Output|", "Input/Output"), "input/output");

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx(pgettext("Preferences|", "Advanced"), ImGuiTreeNodeFlags_DefaultOpen)) {
        selectableTreeNode(pgettext("Preferences|Advanced|", "Graphics"), "graphics");

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

    if (ImGui::Button(_("Save"))) {
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

#pragma region General

void Marmalade::GUI::Preferences::drawGeneralLoggingPane() {
    Components::Form form;

    std::vector<std::string> levels{"Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off"};

    form.AddEntry(Components::FormEntry(_("Log Level"), reinterpret_cast<int*>(&EngineConfig::GetStoredConfig().logLevel))
                          .AsDropdown(levels)
                          .SetRequiresRestartWarning(true));

    form.DrawForm();
}

void Marmalade::GUI::Preferences::drawGeneralProjectsPane() {
    Components::Form form;

    form.AddEntry(Components::FormEntry(_("Default Project Path"), &EngineConfig::GetStoredConfig().defaultProjectPath));
    form.AddEntry(Components::FormEntry(_("Show Welcome Screen on Startup"), &EngineConfig::GetStoredConfig().appearance.showWelcomeScreen));

    form.DrawForm();
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

    ImGui::Checkbox(_("File Associations"), &fileAssoc);
    ImGui::Checkbox(_("Launcher"), &launcher);

    if (ImGui::Button(_("Add Integrations"))) {
        Application::Integration::AddSystemIntegrations(static_cast<it>(type));
    }
}

void Marmalade::GUI::Preferences::drawGeneralPluginsPane() {
    auto loadedPlugins = PluginLoader::GetInstance().GetLoadedPlugins();

    ImGui::Text(ngettext("1 plugin loaded", "%d plugins loaded", loadedPlugins.size()), loadedPlugins.size());

    static int selected = -1;
    auto pluginsDir = ConfigUtil::GetConfigDirectory() / "plugins";
    if (ImGui::BeginTable("##Plugins", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn(_("File Name"));
        ImGui::TableSetupColumn(_("Type"));
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < loadedPlugins.size(); ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            auto relativePath = std::filesystem::relative(loadedPlugins[i].Path, pluginsDir).string();
            const bool isSelected = (selected == (int) i);
            if (ImGui::Selectable(relativePath.c_str(), isSelected)) selected = (int) i;

            ImGui::TableSetColumnIndex(1);
            ImGui::Text(loadedPlugins[i].Type == PluginType_WASM ? _("WASM") : _("Native"));
        }
        ImGui::EndTable();
    }
}

#pragma endregion

#pragma region User Interface

void Marmalade::GUI::Preferences::drawUIAppearancePane() {
    Components::Form form;

    form.AddEntry(Components::FormEntry(_("ImGui Viewports"), &EngineConfig::GetStoredConfig().appearance.viewports)
                          .SetRequiresRestartWarning(true));
    form.AddEntry(Components::FormEntry(_("Theme File"), &EngineConfig::GetStoredConfig().appearance.themeFile)
                          .SetRequiresRestartWarning(true));

    Components::CustomEntry styleEditorEntry;
    styleEditorEntry.DrawCallback = [] {
        if (ImGui::Button(ICON_WITH_TEXT(ICON_CI_EDIT, _("Open Style Editor")))) {
            WindowManager::GetInstance().showStyleEditor = true;
        }
    };
    form.AddEntry(Components::FormEntry(_("Current Style"), &styleEditorEntry));

    static auto backgroundCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().appearance.backgroundColor);
    form.AddEntry(Components::FormEntry(_("Background Color"), &backgroundCol)
                          .SetControlType(Components::ControlType::ControlType_COLOR_EDIT)
                          .SetValueChangeCallback([] {
                              EngineConfig::GetStoredConfig().appearance.backgroundColor = ImGui::ColorConvertFloat4ToU32(backgroundCol);
                          }));

    form.AddEntry(Components::FormEntry(_("Scale Factor"), &EngineConfig::GetStoredConfig().appearance.scaleFactor)
                          .SetDisabled(EngineConfig::GetStoredConfig().appearance.useSystemScaleFactor)
                          .SetRange(0.25f, 5.0f));
    form.AddEntry(Components::FormEntry(_("Use system scale cactor"), &EngineConfig::GetStoredConfig().appearance.useSystemScaleFactor)
                          .SetRequiresRestartWarning(true));

    form.AddEntry(Components::FormEntry(_("Invert message box button order"), &EngineConfig::GetStoredConfig().appearance.alternativeButtonOrder));

    form.AddEntry(Components::FormEntry(_("Use native menu bar on macOS"), &EngineConfig::GetStoredConfig().appearance.useNativeMenubar));

    form.DrawForm();
}

void Marmalade::GUI::Preferences::drawUIWorkspacesPane() {
    ImGui::Checkbox(_("Automatic Workspace Switching"), &EngineConfig::GetStoredConfig().appearance.automaticWorkspaceSwitching);
    ImGui::SetItemTooltip(_("Automatically switch to a different workspace topology when a monitor is connected or disconnected."));
}

void Marmalade::GUI::Preferences::drawUIProjectBrowserPane() {
    Components::Form form;

    static auto assetsCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorAssets);
    form.AddEntry(Components::FormEntry(_("Assets"), &assetsCol)
                          .SetControlType(Components::ControlType::ControlType_COLOR_EDIT)
                          .SetValueChangeCallback([] {
                              EngineConfig::GetStoredConfig().projectBrowser.colorAssets = ImGui::ColorConvertFloat4ToU32(assetsCol);
                          }));

    static auto dataCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorData);
    form.AddEntry(Components::FormEntry(_("Data"), &dataCol)
                          .SetControlType(Components::ControlType::ControlType_COLOR_EDIT)
                          .SetValueChangeCallback([] {
                              EngineConfig::GetStoredConfig().projectBrowser.colorData = ImGui::ColorConvertFloat4ToU32(dataCol);
                          }));

    static auto srcCol = ImGui::ColorConvertU32ToFloat4(EngineConfig::GetStoredConfig().projectBrowser.colorSrc);
    form.AddEntry(Components::FormEntry(_("Src"), &srcCol)
                          .SetControlType(Components::ControlType::ControlType_COLOR_EDIT)
                          .SetValueChangeCallback([] {
                              EngineConfig::GetStoredConfig().projectBrowser.colorSrc = ImGui::ColorConvertFloat4ToU32(srcCol);
                          }));

    form.DrawForm();
}

void Marmalade::GUI::Preferences::drawUILocalizationPane() {
    Components::Form form;

    form.AddEntry(Components::FormEntry(_("Language"), &EngineConfig::GetStoredConfig().appearance.language)
        .SetRequiresRestartWarning(true));

    form.DrawForm();
}

#pragma endregion

#pragma region Audio

void Marmalade::GUI::Preferences::drawAudioInputOutputPane() {
    auto& engineConfig = EngineConfig::GetStoredConfig();
    auto& audioManager = ::GET_APP.audioManager->GetInstance();

    std::vector<std::string> deviceList = audioManager.GetAvailableDevices();

    std::vector<const char*> cstrings;
    for (const auto& str: deviceList) {
        cstrings.push_back(str.c_str());
    }

    static int selectedDeviceIndex = 0;
    for (size_t i = 0; i < deviceList.size(); ++i) {
        if (deviceList[i] == engineConfig.audioOutputDevice) {
            selectedDeviceIndex = static_cast<int>(i);
            break;
        }
    }

    if (ImGui::Combo(_("Output device"), &selectedDeviceIndex, cstrings.data(), cstrings.size())) {
        const std::string& selectedDevice = deviceList[selectedDeviceIndex];

        audioManager.SetCurrentDevice(selectedDevice.c_str());
        engineConfig.audioOutputDevice = selectedDevice;

        LOG_INFO("Selected device: {}", selectedDevice);
    }
}

#pragma endregion

#pragma region Advanced

void Marmalade::GUI::Preferences::drawAdvancedGraphicsPane() {
    ImGui::Text(_("Graphics system for the user interface:"));
    requiresRestartWarning();

    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ICON_WITH_TEXT(ICON_CI_WARNING, _("Do not change these unless you know what you're doing!")));

#undef interface
    Components::Form form;

    form.AddEntry(Components::FormEntry(_("System"), &EngineConfig::GetStoredConfig().interface.graphicsSystem));
    form.AddEntry(Components::FormEntry(_("Version"), &EngineConfig::GetStoredConfig().interface.graphicsVersion));

    form.DrawForm();
}

#pragma endregion

void Marmalade::GUI::Preferences::requiresRestartWarning() {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ICON_WITH_TEXT(ICON_CI_WARNING, _("Requires restart")));
}
