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

#include "welcomescreen.h"

#include "../../application/application.h"
#include "../../application/recents.h"
#include "../../project/projectmanager.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>

void Marmalade::GUI::WelcomeScreen::Draw() {
    ImGui::SetNextWindowSize(ImVec2(700.0f, 400.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowFocus();

    WINDOW_BEGIN_MODAL(ICON_CI_STAR " Welcome", ImGuiWindowFlags_None)

    drawSplit();

    WINDOW_END_MODAL()
}

void Marmalade::GUI::WelcomeScreen::drawLeftPane() {
    static std::vector<LeftPaneItem> items = {
            {ICON_CI_ADD " Create Project", createProject},
            {ICON_CI_FOLDER_OPENED " Open Project", openProject}};

    if (ImGui::BeginListBox("WelcomeScreenLeftList", ImGui::GetContentRegionAvail())) {
        int i = 0;
        for (auto& item: items) {
            ImGui::PushID(("WelcomeScreenLeftList" + std::to_string(i)).c_str());

            bool selected = (leftPaneSelected == i);
            if (ImGui::Selectable(item.name.c_str(), selected)) {
                leftPaneSelected = i;

                items[i].action();
            }

            ImGui::Separator();
            ImGui::PopID();
            i++;
        }
        ImGui::EndListBox();
    }
}

void Marmalade::GUI::WelcomeScreen::drawRightPane() {
    ImGui::Text("Recent Projects");
    ImGui::Spacing();

    if (ImGui::BeginListBox("WelcomeScreenRightList", ImGui::GetContentRegionAvail())) {
        int i = 0;
        for (auto& item: Recents::recentsConfig.projects) {
            ImGui::PushID(("WelcomeScreenRightList" + std::to_string(i)).c_str());

            bool selected = (rightPaneSelected == i);
            if (ImGui::Selectable(item.name.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick)) {
                rightPaneSelected = i;

                if (ImGui::IsMouseDoubleClicked(0)) {
                    if (Application::GetInstance().OpenProject(item.path)) visible = false;
                }
            }

            ImGui::Text("%s", item.path.c_str());

            ImGui::Separator();
            ImGui::PopID();
            i++;
        }
        ImGui::EndListBox();
    }
}

void Marmalade::GUI::WelcomeScreen::drawSplit() {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static float split_ratio = 0.3f;
    static float min_size = 50.0f;

    float left_width = area.x * split_ratio;
    float right_width = area.x - left_width - 4.0f;

    left_width = ImMax(left_width, min_size);
    right_width = ImMax(right_width, min_size);

    ImGui::BeginChild("WelcomeLeftPane", ImVec2(left_width, area.y), true);
    drawLeftPane();
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::InvisibleButton("##WelcomeSplitter", ImVec2(8.0f, area.y), ImGuiButtonFlags_None);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    if (ImGui::IsItemActive()) {
        ImGuiIO& io = ImGui::GetIO();
        split_ratio += io.MouseDelta.x / area.x;
        split_ratio = ImClamp(split_ratio, min_size / area.x, 1.0f - min_size / area.x);
    }

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("WelcomeRightPane", ImVec2(right_width, area.y), true);
    drawRightPane();
    ImGui::EndChild();
}

void Marmalade::GUI::WelcomeScreen::createProject() {
    spdlog::info("Create project");
}

void Marmalade::GUI::WelcomeScreen::openProject() {
    spdlog::info("Open project");
}
