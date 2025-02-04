#include "settings.h"

#include "../../application/config.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::Settings::Draw() {
    ImGui::Begin(ICON_CI_SETTINGS_GEAR " Settings", &visible);

    ImGui::Checkbox("ImGui Viewports", &Marmalade::Config::engineConfig.Viewports);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ICON_CI_WARNING " Requires restart");

    if (ImGui::Button("Save")) {
        Marmalade::Config::SaveEngineConfig();
    }

    ImGui::End();
}
