#include "packagemanager.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <git2.h>
#include <iostream>
#include <sstream>

enum PackageManagerTab {
    PackageManagerTab_All,
    PackageManagerTab_Installed,
    PackageManagerTab_Available
};

void DrawLeftPane(PackageManagerTab tab) {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static const char* items[] = { "Package 1", "Package 2", "Package 3", "Package 4", "Package 5" };
    static bool selected[IM_ARRAYSIZE(items)] = { false };

    std::stringstream list_id;
    list_id << "##PackageManagerList" << tab;

    if (ImGui::BeginListBox(list_id.str().c_str(), area))
    {
        for (int i = 0; i < IM_ARRAYSIZE(items); i++)
        {
            ImGui::PushID(i);
            ImGui::Checkbox(items[i], &selected[i]);
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }
}

void DrawRightPane(PackageManagerTab tab) {
    ImGui::Text("Package details");
}

void DrawSplit(PackageManagerTab tab, float bottom_bar_height) {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static float split_ratio = 0.3f;
    static float min_size = 50.0f;

    float left_width = area.x * split_ratio;
    float right_width = area.x - left_width - 4.0f;

    left_width = ImMax(left_width, min_size);
    right_width = ImMax(right_width, min_size);

    ImGui::BeginChild("PackageManagerLeftPane", ImVec2(left_width, area.y - bottom_bar_height), true);
    DrawLeftPane(tab);
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::InvisibleButton("##PackageManagerSplitter", ImVec2(8.0f, area.y - bottom_bar_height), ImGuiButtonFlags_None);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    // Handle resizing
    if (ImGui::IsItemActive())
    {
        ImGuiIO &io = ImGui::GetIO();
        split_ratio += io.MouseDelta.x / area.x;
        split_ratio = ImClamp(split_ratio, min_size / area.x, 1.0f - min_size / area.x);
    }

    // Right Pane
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("PackageManagerRightPane", ImVec2(right_width, area.y - bottom_bar_height), true);
    DrawRightPane(tab);
    ImGui::EndChild();
}

void DrawBottomBar(float height) {
    float window_width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("PackageManagerBottomBar", ImVec2(window_width, height));

    ImGui::Button("Refresh Local Database");

    ImGui::SameLine(window_width - 50);
    ImGui::Button("Apply");

    ImGui::EndChild();
}

void PackageManager::Show(bool* p_open) {
    const float BOTTOM_BAR_HEIGHT = 22.0f;

    ImGui::Begin("Package Manager", p_open, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
    if (ImGui::BeginTabBar("PackageManagerTabs") ) {
        if (ImGui::BeginTabItem("All")) {
            DrawSplit(PackageManagerTab_All, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Installed")) {
            DrawSplit(PackageManagerTab_Installed, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Available")) {
            DrawSplit(PackageManagerTab_Available, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    DrawBottomBar(BOTTOM_BAR_HEIGHT);

    ImGui::End();
}
