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

#include "../../application/util.h"
#include "../../application/application.h"
#include "../../application/plugins/interfaceimpl.h"

#include <glm/common.hpp>

#include <imgui.h>
#include <imnodes.h>

#include <IconsCodicons.h>
#include <ImGuiFileDialog.h>

void Marmalade::GUI::Animation::Draw() {
    WINDOW_BEGIN(ICON_CI_DEVICE_CAMERA_VIDEO " Animation", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse)

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

    WINDOW_END()
}

void Marmalade::GUI::Animation::DrawNodeArea() {
    ImGui::Text("Node View");
    ImGui::Separator();

    if (driver) {
        for (const auto& anims : driver->animations) { // Create nodes for already existing and new nodes
            const auto& animation = anims.second;

            if (createdNodes.find(animation.get()) != createdNodes.end()) { // If node is already created, do not create it again
                continue;
            }

            SequenceNode newNode;
            newNode.id = nextNodeId++;
            newNode.name = RemoveFileExtention(animation->name); // Remove the '.animseq' file extension from node name
            newNode.position = ImVec2(200.0f, 100.0f);
            nodes.push_back(newNode);

            createdNodes.insert(animation.get());
        }
    }

    if (nodes.empty()) {
        ImGui::Text("No nodes created.");
        return;
    }

    int startAttr, endAttr;
    if (ImNodes::IsLinkCreated(&startAttr, &endAttr)) { // Create a new link from the start node to the end / target node
        int startNodeId = startAttr / 2;
        int endNodeId = endAttr / 2;

        if (startNodeId != endNodeId) {
            transitions.push_back({startNodeId, endNodeId});
        }
    }

    int linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) { // Destroy link from the start node to the end / target node
        auto it = std::remove_if(transitions.begin(), transitions.end(),
                                 [linkId](const NodeTransition& link) { return link.startNodeID == linkId ||
                                                                               link.targetNodeID == linkId; });
        transitions.erase(it, transitions.end());
    }

    ImNodes::BeginNodeEditor();

    for (auto& node : nodes) {
        if (node.id <= 0) {
            LOG_ERROR("Invalid node ID: {}", node.id);
            continue;
        }

        ImNodes::BeginNode(node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node.name.c_str());
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(node.id * 2);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(node.id * 2 + 1);
        ImGui::Text("Out");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }

    for (auto& trans : transitions) { // Render node transition links
        ImNodes::Link(trans.startNodeID, trans.startNodeID * 2 + 1, trans.targetNodeID * 2);
    }

    ImNodes::EndNodeEditor();
}

void Marmalade::GUI::Animation::DrawTimeline() {
    ImGui::Text("Timeline");
    ImGui::Separator();

    if (!driver) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No animation driver found, select a entity with an animator component with a driver");
        return;
    }

    float availableWindowWidth = ImGui::GetContentRegionAvail().x;
    float comboWidth = availableWindowWidth * 0.7f;
    float sliderWidth = availableWindowWidth * 0.3f;

    ImGui::PushItemWidth(comboWidth);
    DrawSequenceSelector();
    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderFloat("##ZoomSlider", &_timelineZoom, 0.1f, 20.0f, "Zoom: %.1fx");
    ImGui::PopItemWidth();

    ImGui::BeginChild("Timeline", ImVec2(0, 0), true);

    if (!animation) { ImGui::EndChild(); return; }

    auto deltaTime = static_cast<float>(Application::GetInstance().profiler.GetDeltaTime());

    CreatePlaybackControls(*animation, deltaTime);

    ImGui::SameLine();

    ImGui::InputFloat("##Duration", &animation->length, 0.1, 3600.0f, "Duration: %.1fx"); // Allow up to an hour-long animations

    DrawRuler(animation->length, _timelineZoom);

    DrawFrameMarkers(animation.get(), _timelineZoom);

    DrawScrubber(playbackTime, animation->length, _timelineZoom);

    ImGui::EndChild();
}

void Marmalade::GUI::Animation::DrawPreviewWindow() {
    ImGui::Text("Preview");
    ImGui::Separator();

    // TODO
}

void Marmalade::GUI::Animation::DrawInspector() {
    ImGui::Text("Inspector");
    ImGui::Separator();

    // TODO
}

bool Marmalade::GUI::Animation::DrawSplitter(const char* id, float* size, float minSize, float maxSize, bool isVertical) {
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

void Marmalade::GUI::Animation::DrawTopSection(float topHeight) {
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

void Marmalade::GUI::Animation::DrawRuler(float length, float zoom) {
    if (!animation) { return; }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    float timelineWidth = length * 100.0f * zoom;

    float stepSize = (zoom >= MIN_ZOOM_THRESHOLD) ? 0.1f : 1.0f;
    float labelStep = (zoom >= MIN_ZOOM_THRESHOLD) ? 0.5f : 1.0f;

    for (float t = 0.0f; t <= length + stepSize; t += stepSize) { // Draw ruler line with length appropriate to the zoom level
        float xPos = cursorPos.x + t * 100.0f * zoom;
        if (t - floor(t) < 0.0001f || zoom >= MIN_ZOOM_THRESHOLD) {
            drawList->AddLine(ImVec2(xPos, cursorPos.y), ImVec2(xPos, cursorPos.y + 10), IM_COL32(200, 200, 200, 255), 1.0f);

            if (fmod(t, labelStep) < 0.0001f) { // Draw time labels
                std::string label = (zoom >= MIN_ZOOM_THRESHOLD) ? std::to_string(static_cast<int>(t * 1000)) + "ms" : std::to_string(static_cast<int>(t)) + "s";
                drawList->AddText(ImVec2(xPos + 2, cursorPos.y + 12), IM_COL32(255, 255, 255, 255), label.c_str());
            }
        }
    }

    ImGui::Dummy(ImVec2(timelineWidth, 30.0f));
}

void Marmalade::GUI::Animation::DrawFrameMarkers(Marmalade::Animation::AnimationSequence* sequence, float zoom) {
    if (!sequence || sequence->frames.empty()) return;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    for (int i = 0; i < sequence->frames.size(); ++i) { // Draw frame markers
        float framePos = cursorPos.x + playbackTime * 100.0f * zoom;
        drawList->AddLine(ImVec2(framePos, cursorPos.y), ImVec2(framePos, cursorPos.y + 30), IM_COL32(255, 200, 100, 255), 2.0f);
    }
}

void Marmalade::GUI::Animation::DrawScrubber(float& playbackTime, float length, float zoom) {
    if (!animation) { return; }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    float scrubberY = cursorPos.y - 30.0f;
    float scrubberX = cursorPos.x + playbackTime * 100.0f * zoom;

    drawList->AddLine(ImVec2(scrubberX, scrubberY), ImVec2(scrubberX, scrubberY + 30.0f), IM_COL32(255, 0, 0, 255), 2.0f);
    drawList->AddRectFilled(ImVec2(scrubberX - 5.0f, scrubberY + 30.0f), ImVec2(scrubberX + 5.0f, scrubberY + 30.0f + SCRUBBER_HEIGHT), IM_COL32(255, 0, 0, 255));

    if (ImGui::IsMouseDragging(0) && ImGui::IsItemHovered()) {
        float deltaX = ImGui::GetMouseDragDelta().x;
        playbackTime += deltaX / (100.0f * zoom * 10.0f);

        playbackTime = glm::clamp(playbackTime, 0.0f, length);
    }
}

void Marmalade::GUI::Animation::CreatePlaybackControls(Marmalade::Animation::AnimationSequence& sequence, float deltaTime) {
    if (!animation) { return; }

    if (ImGui::Button(_isPlaying ? ICON_CI_STOP " Stop" : ICON_CI_PLAY " Play")) {
        _isPlaying = !_isPlaying;
    }

    if (!_isPlaying) {
        playbackTime = 0.0f;
    }

    if (_isPlaying) {
        playbackTime += deltaTime;
        if (playbackTime > sequence.length) {
            if (sequence.loop) {
                playbackTime = 0.0f;
            } else {
                playbackTime = sequence.length;
                _isPlaying = false;
            }
        }
    }
    ImGui::Text("Playback Time: %.2f / %.2f", playbackTime, sequence.length);
}

void Marmalade::GUI::Animation::DrawSequenceSelector() {
    static int selected = 0;
    static std::vector<std::unique_ptr<Marmalade::Animation::AnimationSequence>> listedSequences;

    listedSequences.clear();

    for (const auto& [name, pathStr] : driver->storedConfig.sequences) {
        std::filesystem::path path = pathStr;

        if (std::filesystem::exists(path)) {
            auto anim = std::make_unique<Marmalade::Animation::AnimationSequence>(path, name);
            listedSequences.push_back(std::move(anim));
        }
    }

    if (ImGui::BeginCombo("##SequenceSelector", listedSequences.empty() ? "No Animations available" : listedSequences[selected]->name.c_str())) {
        for (int i = 0; i < listedSequences.size(); ++i) {
            bool isSelected = (selected == i);

            if (ImGui::Selectable(listedSequences[i]->name.c_str(), isSelected)) {
                selected = i;
                animation = std::move(listedSequences[i]);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        if (ImGui::Selectable(ICON_CI_PLUS "Create Animation")) {
            IGFD::FileDialogConfig config;
            config.path = EngineConfig::GetStoredConfig().defaultProjectPath;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("CreateAnimation", "Create New Animation", ".animseq", config);
        }

        if (ImGuiFileDialog::Instance()->Display("CreateAnimation")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string animationNameWithExt = ImGuiFileDialog::Instance()->GetCurrentFileName(true);
                std::string animationName = RemoveFileExtention(animationNameWithExt);

                LOG_DEBUG("Creating new animation: {}", animationName);

                auto newAnimation = std::make_shared<Marmalade::Animation::AnimationSequence>(filePath, animationNameWithExt);
                newAnimation->uuid = Marmalade::Util::GenerateUUIDv4();
                newAnimation->SaveConfig();

                listedSequences.push_back(std::make_unique<Marmalade::Animation::AnimationSequence>(*newAnimation));
                selected = listedSequences.size() - 1;
                animation = std::make_unique<Marmalade::Animation::AnimationSequence>(*listedSequences[selected]);

                if (driver) {
                    driver->storedConfig.sequences[animationName] = filePath;
                    driver->storedConfig.uuid = Util::GenerateUUIDv4();
                    driver->storedConfig.name = animationName;
                    driver->animations[animationName] = newAnimation;

                    if (!driver->currentSequence) {
                        driver->currentSequence = newAnimation;
                        driver->storedConfig.currentSequence = animationName;
                    }
                    driver->SaveConfig();
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::EndCombo();
    }
}
