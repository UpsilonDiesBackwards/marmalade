#include "settings.h"

#include "../src/application/application.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <IconsCodicons.h>

void Marmalade::GUI::ProjectSettings::Draw() {
    Application& app = Application::GetInstance();
    auto currentProject = app.GetCurrentProject();

    ImGui::SetNextWindowSize(ImVec2(1080, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_CI_SETTINGS " Project Settings", &visible, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

    if (app.GetCurrentProject() == nullptr) {
        spdlog::error("Can not open project settings! A project is not currently opened.");
        return;
    };

    if (ImGui::BeginTabBar("ProjectSettingsTabs")) {
        if (ImGui::BeginTabItem("Project")) {
            drawProjectSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Build")) {
            drawBuildSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Debug")) {
            drawDebugSettings();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (ImGui::Button("Save")) {
        currentProject->settings.SaveProjectSettings();
    }

    ImGui::End();
}

void Marmalade::GUI::ProjectSettings::drawProjectSettings() {
    Application& app = Application::GetInstance();
    auto currentProject = app.GetCurrentProject();

    char projectNameC[64];
    std::strcpy(projectNameC, currentProject->settings.projectSettings.ProductName.c_str());

    char companyNameC[64];
    std::strcpy(companyNameC, currentProject->settings.projectSettings.CompanyName.c_str());

    char projectDescriptionC[1024];
    std::strcpy(projectDescriptionC, currentProject->settings.projectSettings.Description.c_str());

    char projectVersionC[32];
    std::strcpy(projectVersionC, currentProject->settings.projectSettings.Version.c_str());

    if (ImGui::InputText("Project Name", projectNameC, IM_ARRAYSIZE(projectNameC))) {
        currentProject->settings.projectSettings.ProductName = projectNameC;
    }

    if (ImGui::InputText("Company Name", companyNameC, IM_ARRAYSIZE(companyNameC))) {
        currentProject->settings.projectSettings.CompanyName = companyNameC;
    }

    if (ImGui::InputText("Description", projectDescriptionC, IM_ARRAYSIZE(projectDescriptionC))) {
        currentProject->settings.projectSettings.Description = projectDescriptionC;
    }

    if (ImGui::InputText("Version", projectVersionC, IM_ARRAYSIZE(projectVersionC))) {
        currentProject->settings.projectSettings.Version = projectVersionC;
    }
}

void Marmalade::GUI::ProjectSettings::drawBuildSettings() {
    ImGui::Text("Build settings");
}

void Marmalade::GUI::ProjectSettings::drawDebugSettings() {
    ImGui::Text("Debug settings");
}
