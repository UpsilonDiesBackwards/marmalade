
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

#ifndef MARMALADE_BACKGROUNDLABEL_H
#define MARMALADE_BACKGROUNDLABEL_H

#include <imgui.h>
#include <string>

namespace Marmalade::GUI::Components {

    inline void Draw(const char* text, const ImVec4& bgColor, const ImVec4& textColor = ImVec4(1, 1, 1, 1), float rounding = 3.0f, float padding = 3.0f) {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 paddingVec = ImVec2(padding, padding * 0.5f);

        ImGui::GetWindowDrawList()->AddRectFilled(
                cursorPos,
                ImVec2(cursorPos.x + textSize.x + paddingVec.x * 2, cursorPos.y + textSize.y + paddingVec.y * 2),
                ImGui::ColorConvertFloat4ToU32(bgColor),
                rounding
        );

        ImGui::SetCursorScreenPos(ImVec2(
                cursorPos.x + paddingVec.x,
                cursorPos.y + paddingVec.y
                ));

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertFloat4ToU32(textColor));
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos(cursorPos);
        ImGui::Dummy(ImVec2(textSize.x + paddingVec.x * 2, textSize.y + paddingVec.y * 2));
    }

    inline void DrawInlineLabelWithBackground(const char* text, const ImVec4& bgColor, const ImVec4& textColor = ImVec4(1, 1, 1, 1), float rounding = 3.0f, float padding = 3.0f) {
        Draw(text, bgColor, textColor, rounding, padding);
        ImGui::SameLine();
    }
}

#endif // MARMALADE_BACKGROUNDLABEL_H

