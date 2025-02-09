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
