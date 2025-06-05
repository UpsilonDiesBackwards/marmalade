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

#include "dialog.h"

#include "../application/config/engineconfig.h"
#include "../application/i18n.h"

void Marmalade::GUI::DialogButtons::AddButton(const char* primaryLbl, const std::function<void()>& primaryFunc) {
    const float width = ImGui::CalcTextSize(primaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (width));

    if (ImGui::Button(primaryLbl)) {
        primaryFunc();
    }
}

void Marmalade::GUI::DialogButtons::AddTwoButtons(const char* primaryLbl, const std::function<void()>& primaryFunc, const char* secondaryLbl, const std::function<void()>& secondaryFunc) {
    const float primaryWidth = ImGui::CalcTextSize(primaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;
    const float secondaryWidth = ImGui::CalcTextSize(secondaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (primaryWidth + ImGui::GetStyle().ItemSpacing.x + secondaryWidth));

    const bool invertOrder = shouldInvertOrder();
    if (!invertOrder) {
        // Windows
        if (ImGui::Button(primaryLbl)) {
            primaryFunc();
        }
        ImGui::SameLine();
    }
    if (ImGui::Button(secondaryLbl)) {
        secondaryFunc();
    }
    if (invertOrder) {
        // Linux
        ImGui::SameLine();
        if (ImGui::Button(primaryLbl)) {
            primaryFunc();
        }
    }
}

void Marmalade::GUI::DialogButtons::AddThreeButtons(const char* primaryLbl, const std::function<void()>& primaryFunc, const char* secondaryLbl, const std::function<void()>& secondaryFunc, const char* ternaryLbl, const std::function<void()>& ternaryFunc) {
    const float primaryWidth = ImGui::CalcTextSize(primaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;
    const float secondaryWidth = ImGui::CalcTextSize(secondaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;
    const float ternaryWidth = ImGui::CalcTextSize(ternaryLbl).x + ImGui::GetStyle().FramePadding.x * 2;

#ifdef __APPLE__
    // macOS has ternary button on left
#else
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (primaryWidth + ImGui::GetStyle().ItemSpacing.x + secondaryWidth + ImGui::GetStyle().ItemSpacing.x + ternaryWidth));
#endif

    const bool invertOrder = shouldInvertOrder();

#ifdef __APPLE__
    // macOS "left" button
    if (ImGui::Button(invertOrder ? secondaryLbl : primaryLbl)) {
        invertOrder ? secondaryFunc() : primaryFunc();
    }
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (primaryWidth + ImGui::GetStyle().ItemSpacing.x + ternaryWidth));

    if (ImGui::Button(ternaryLbl)) {
        ternaryFunc();
    }
    ImGui::SameLine();
#else
    if (!invertOrder) {
        // Windows
        if (ImGui::Button(primaryLbl)) {
            primaryFunc();
        }
        ImGui::SameLine();
    } else {
        // Linux
        if (ImGui::Button(ternaryLbl)) {
            ternaryFunc();
        }
        ImGui::SameLine();
    }
#endif

#ifdef __APPLE__
    // macOS
    if (ImGui::Button(invertOrder ? primaryLbl : secondaryLbl)) {
        invertOrder ? primaryFunc() : secondaryFunc();
    }
#else
    if (!invertOrder) {
        // Windows
        if (ImGui::Button(secondaryLbl)) {
            secondaryFunc();
        }
        ImGui::SameLine();
    } else {
        // Linux
        if (ImGui::Button(secondaryLbl)) {
            secondaryFunc();
        }
        ImGui::SameLine();
    }
#endif

#ifndef __APPLE__
    if (!invertOrder) {
        // Windows
        if (ImGui::Button(ternaryLbl)) {
            ternaryFunc();
        }
    } else {

        // Linux
        if (ImGui::Button(primaryLbl)) {
            primaryFunc();
        }
    }
#endif
}

void Marmalade::GUI::DialogButtons::Ok(const std::function<void()>& okFunc) {
    AddButton(_("OK"), okFunc);
}

void Marmalade::GUI::DialogButtons::OkCancel(const std::function<void()>& okFunc, const std::function<void()>& cancelFunc) {
    AddTwoButtons(_("OK"), okFunc, _("Cancel"), cancelFunc);
}

void Marmalade::GUI::DialogButtons::YesNo(const std::function<void()>& yesFunc, const std::function<void()>& noFunc) {
    AddTwoButtons(_("Yes"), yesFunc, _("No"), noFunc);
}

void Marmalade::GUI::DialogButtons::YesNoCancel(const std::function<void()>& yesFunc, const std::function<void()>& noFunc, const std::function<void()>& cancelFunc) {
    AddThreeButtons(_("Yes"), yesFunc, _("No"), noFunc, _("Cancel"), cancelFunc);
}

bool Marmalade::GUI::DialogButtons::shouldInvertOrder() {
    bool invertOrder =
#ifdef _WIN32
            false;
#else
            true;
#endif
    invertOrder ^= EngineConfig::GetStoredConfig().appearance.alternateButtonOrder;
    return invertOrder;
}
