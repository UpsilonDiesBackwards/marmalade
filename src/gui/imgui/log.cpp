#include "log.h"

#include "../../application/application.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::Log::Draw() {
    static const ImVec4 COLOR_YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 COLOR_RED(1.0f, 0.0f, 0.0f, 1.0f);
    static const ImVec4 COLOR_GRAY(0.6f, 0.6f, 0.6f, 1.0f);

    ImGui::Begin(ICON_CI_FILE_TEXT " Engine Log", &visible);
    {
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

        ImGui::End();
    }

    ImGui::Begin(ICON_CI_FILE_TEXT " Game Log", &visible);
    {
        ImGui::End();
    }
}
