
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

#include "animation.h"

#include "imgui.h"

#include "IconsCodicons.h"

void Marmalade::GUI::AnimationTimeline::Draw() {
    ImGui::Begin(ICON_CI_DEVICE_CAMERA_VIDEO " Animation Timeline", &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

    static float topSectionHeight = 300.0f;
    float minHeight = 100.0f;
    float maxHeight = ImGui::GetContentRegionAvail().y - minHeight;

    DrawTopSection(topSectionHeight);

    // Primary Splitter between top and bottom section
    if (DrawSplitter("PrimarySplitter", &topSectionHeight, minHeight, maxHeight, false)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    }

    // Animation timeline
    DrawTimeline();

    ImGui::End();
}

void Marmalade::GUI::AnimationTimeline::DrawNodeArea() {
    ImGui::Text("Node View");
    ImGui::Separator();
}

void Marmalade::GUI::AnimationTimeline::DrawTimeline() {
    ImGui::Text("Timeline");
    ImGui::Separator();

    ImGui::BeginChild("Timeline", ImVec2(0, 0), true);



    ImGui::EndChild();
}

void Marmalade::GUI::AnimationTimeline::DrawPreviewWindow() {
    ImGui::Text("Preview");
    ImGui::Separator();
}

void Marmalade::GUI::AnimationTimeline::DrawInspector() {
    ImGui::Text("Inspector");
    ImGui::Separator();
}

bool Marmalade::GUI::AnimationTimeline::DrawSplitter(const char* id, float* size, float minSize, float maxSize, bool isVertical) {
    ImVec2 cursorPos = ImGui::GetCursorPos();
    float thickness = 5.0f;

    ImVec2 splitterSize = isVertical ? ImVec2(thickness, -1.0f) : ImVec2(-1.0f, thickness);
    ImGui::SetCursorPos(cursorPos);

    ImGui::PushID(id);
    ImGui::InvisibleButton("##Splitter", splitterSize);

    bool resizing = ImGui::IsItemActive();

    // We want to draw a line to indicate that the space is an interactable splitter
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    float padding = 420.0f;
    ImU32 color = ImGui::GetColorU32(IM_COL32(150, 150, 150, 200));

    if (isVertical) { // Draw vertical line if splitter is vertical
        float midX = (min.x + max.x) * 0.5f;
        float lineStartY = min.y + padding;
        float lineEndY = min.y - padding;
        drawList->AddLine(ImVec2(midX, lineStartY), ImVec2(midX, lineEndY), color, 1.0f);
    } else { // Draw horizontal line if splitter is NOT vertical
        float midY = (min.y + max.y) * 0.5f;
        float lineStartX = min.x + padding;
        float lineEndX = max.x - padding;
        drawList->AddLine(ImVec2(lineStartX, midY), ImVec2(lineEndX, midY), color, 1.0f);
    }

    if (ImGui::IsItemHovered() || resizing) {
        ImGui::SetMouseCursor(isVertical ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);
    }

    if (resizing) {
        float delta = isVertical ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y;
        *size += delta;
        *size = std::clamp(*size, minSize, maxSize);
    }

    ImGui::PopID();
    return resizing;
}

void Marmalade::GUI::AnimationTimeline::DrawTopSection(float topHeight) {
    static float rightPanelWidth = 350.0f;
    static float previewHeight = 90.0f;

    float minPanelHeight = 50.0f;
    float maxPanelHeight = ImGui::GetContentRegionAvail().y - minPanelHeight;

    // Top Section
    ImGui::BeginChild("TopSection", ImVec2(0, topHeight), true);

    ImGui::Columns(2, "TopColumns", true);

    // Node View Column
    float topPanelHeight = ImGui::GetContentRegionAvail().y;
    ImGui::BeginChild("NodeView", ImVec2(0, topPanelHeight), true);
    DrawNodeArea();
    ImGui::EndChild();

    ImGui::NextColumn();

    // Right side dock
    rightPanelWidth = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("TopRightDock", ImVec2(rightPanelWidth, 0), true);

    // Animation preview row
    ImGui::BeginChild("Preview", ImVec2(0, previewHeight), true);
    DrawPreviewWindow();
    ImGui::EndChild();

    if (DrawSplitter("RightDockSplitter", &previewHeight, minPanelHeight, maxPanelHeight, false)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    }

    // Inspector Row
    ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
    DrawInspector();
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::Columns(1);

    ImGui::EndChild();
}
