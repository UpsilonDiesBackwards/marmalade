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

#include "settings.h"

#include "../../application/application.h"
#include "../../project/projectmanager.h"
#include "../../application/plugins/interfaceimpl.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

void Marmalade::GUI::ProjectSettings::Draw() {
    Application& app = Application::GetInstance();

    ImGui::SetNextWindowSize(ImVec2(1080, 720), ImGuiCond_FirstUseEver);
    WINDOW_BEGIN(ICON_CI_SETTINGS " Project Settings", ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse)

    if (app.GetCurrentProject() == nullptr) {
        LOG_ERROR("Can not open project settings! A project is not currently opened.");
        return;
    };

    if (ImGui::BeginTabBar("ProjectSettingsTabs")) {
        if (ImGui::BeginTabItem("Project")) {
            drawProjectSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Graphics")) {
            drawGraphicsSettings();
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
        Marmalade::Project::ProjectManager<>::SaveProject(app.GetCurrentProject());
    }

    WINDOW_END()
}

void Marmalade::GUI::ProjectSettings::drawProjectSettings() {
    Application& app = Application::GetInstance();
    auto currentProject = app.GetCurrentProject();

    char projectNameC[64];
    std::strcpy(projectNameC, currentProject->settings.productName.c_str());

    char companyNameC[64];
    std::strcpy(companyNameC, currentProject->settings.companyName.c_str());

    char projectDescriptionC[1024];
    std::strcpy(projectDescriptionC, currentProject->settings.description.c_str());

    char projectVersionC[32];
    std::strcpy(projectVersionC, currentProject->settings.version.c_str());

    if (ImGui::InputText("Project Name", projectNameC, IM_ARRAYSIZE(projectNameC))) {
        currentProject->settings.productName = projectNameC;
    }

    if (ImGui::InputText("Company Name", companyNameC, IM_ARRAYSIZE(companyNameC))) {
        currentProject->settings.companyName = companyNameC;
    }

    if (ImGui::InputText("Description", projectDescriptionC, IM_ARRAYSIZE(projectDescriptionC))) {
        currentProject->settings.description = projectDescriptionC;
    }

    if (ImGui::InputText("Version", projectVersionC, IM_ARRAYSIZE(projectVersionC))) {
        currentProject->settings.version = projectVersionC;
    }
}

void Marmalade::GUI::ProjectSettings::drawGraphicsSettings() {
    Application& app = Application::GetInstance();
    auto currentProject = app.GetCurrentProject();

    ImGui::Checkbox("Enable MSAA", &currentProject->settings.msaaEnabled);

    if (!currentProject->settings.msaaEnabled) {
        currentProject->settings.msaaSampleCount = 0;
        app.framebuffer->Refresh();
    }

    if (currentProject->settings.msaaEnabled) {
        static int sampleValues[] = {2, 4, 8};
        static int sampleIndex = 1;

        char label[32];
        sprintf(label, "Samples: %d", sampleValues[sampleIndex]);

        if (ImGui::SliderInt("MSAA Sample Count", &sampleIndex, 0, IM_ARRAYSIZE(sampleValues) - 1, label)) {
            currentProject->settings.msaaSampleCount = sampleValues[sampleIndex];
            app.framebuffer->Refresh();
        }

        ImGui::Separator();
    }
}

void Marmalade::GUI::ProjectSettings::drawBuildSettings() {
    ImGui::Text("Build settings");
}

void Marmalade::GUI::ProjectSettings::drawDebugSettings() {
    ImGui::Text("Debug settings");
}