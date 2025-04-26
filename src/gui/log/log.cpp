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

#include "log.h"

#include "../../application/application.h"
#include "../../application/plugins/interfaceimpl.h"

#include "imgui.h"

#include "IconsCodicons.h"

void Marmalade::GUI::Log::Draw() {
    static const ImVec4 COLOR_YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 COLOR_RED(1.0f, 0.0f, 0.0f, 1.0f);
    static const ImVec4 COLOR_GRAY(0.6f, 0.6f, 0.6f, 1.0f);

    WINDOW_BEGIN(ICON_CI_FILE_TEXT " Engine Log", ImGuiWindowFlags_None) {
        if (ImGui::BeginChild("CommandLineScrollingRegion", ImVec2(ImGui::GetWindowWidth(),
                                                                   ImGui::GetWindowHeight() - 60))) {
            for (const auto& line: Application::GetInstance().guiSink->get_items()) {
                if (line.first == spdlog::level::warn && coloredText) {
                    ImGui::TextColored(COLOR_YELLOW, "%s", line.second.c_str());
                } else if ((line.first == spdlog::level::err || line.first == spdlog::level::critical) && coloredText) {
                    ImGui::TextColored(COLOR_RED, "%s", line.second.c_str());
                } else if (line.first == spdlog::level::debug && coloredText) {
                    ImGui::TextColored(COLOR_GRAY, "%s", line.second.c_str());
                } else {
                    ImGui::TextUnformatted(line.second.c_str());
                }
            }

            if (autoScroll) {
                ImGui::SetScrollHereY(1.0f);
            }

            if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
                autoScroll = false;
            }
        }
        ImGui::EndChild();

        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::Checkbox("Colored Text", &coloredText);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options")) {
            ImGui::OpenPopup("Options");
        }

        WINDOW_END()
    }

    WINDOW_BEGIN(ICON_CI_FILE_TEXT " Game Log", ImGuiWindowFlags_None) {
        WINDOW_END()
    }
}
