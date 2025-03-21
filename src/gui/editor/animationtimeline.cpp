
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

#include "animationtimeline.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::AnimationTimeline::Draw() {
    ImGui::Begin(ICON_CI_DEVICE_CAMERA_VIDEO " Animation Timeline", &visible);

    if (ImGui::Button(ICON_CI_ADD " Add Frame")) {
        animation->frames.push_back({});
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_CI_TRASH " Remove Frame") && selectedFrameIndex >= 0) {
        animation->frames.erase(animation->frames.begin() + selectedFrameIndex);
        selectedFrameIndex = -1;
    }

    DrawTimeline();

    ImGui::End();
}

void Marmalade::GUI::AnimationTimeline::DrawTimeline() {
    ImGui::Text("Timeline");

    if (!animation) {
        ImGui::Text("No Animation Sequence available. Select an entity with an animation player to begin");
        return;
    }

    if (animation->frames.empty()) {
        ImGui::Text("No frames available. Add a frame to begin.");
        return;
    }

    ImGui::SliderFloat("Zoom", &timelineZoom, 0.5f, 5.0f, "Zoom: %.fx");

    ImGui::BeginChild("TimelineChild", ImVec2(0, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

    float frameWidth = 50.0f * timelineZoom;
    for (int i = 0; i < animation->frames.size(); ++i) {
        ImGui::PushID(i);

        ImVec2 framePos = ImGui::GetCursorScreenPos();
        ImVec2 frameSize(frameWidth, 60.0f);

        if (ImGui::Selectable(("Frame " + std::to_string(i)).c_str(), selectedFrameIndex == i, 0, frameSize)) {
            selectedFrameIndex = i;
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRect(framePos, ImVec2(framePos.x + frameSize.x, framePos.y + frameSize.y),
                          ImGui::GetColorU32(selectedFrameIndex == i ? IM_COL32(255, 200, 100, 255) : IM_COL32(100, 100, 100, 255)),
                          5.0f);

        ImGui::SameLine(0, 5.0f);
        ImGui::PopID();
    }

    ImGui::EndChild();
}
