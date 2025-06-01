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

#include "saveworkspace.h"

#include "../../application/plugins/interfaceimpl.h"
#include "../../application/i18n.h"

#include <imgui.h>

#include <IconsCodicons.h>

#include <cstring>

void Marmalade::GUI::SaveWorkspaceDialog::Draw() {
    WINDOW_BEGIN_MODAL(ICON_WITH_TEXT(ICON_CI_SAVE, _("Save Changes to Workspace")), ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(_("Changes have been made to the workspace: %s."), workspaceName.c_str());

    ImGui::Text(_("Would you like to save these changes?"));

    if (ImGui::Button(_("Yes"))) {
        callback(false, true);
        visible = false;
    }

    ImGui::SameLine();
    if (ImGui::Button(_("No"))) {
        callback(false, false);
        visible = false;
    }

    ImGui::SameLine();
    if (ImGui::Button(_("Cancel"))) {
        callback(true, false);
        visible = false;
    }

    WINDOW_END_MODAL()
}

void Marmalade::GUI::SaveWorkspaceAsDialog::SetType(Marmalade::GUI::SaveWorkspaceAsDialog::DialogType type) {
    _type = type;
    std::strcpy(_name, "");
}

void Marmalade::GUI::SaveWorkspaceAsDialog::Draw() {
    std::string title;

    if (_type == DialogType_SAVE_AS) {
        title = ICON_WITH_TEXT(ICON_CI_SAVE, _("Save to New Workspace"));
    } else if (_type == DialogType_DUPLICATE) {
        title = ICON_WITH_TEXT(ICON_CI_SAVE, _("Duplicate Workspace"));
    }

    WINDOW_BEGIN_MODAL(title.c_str(), ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(_("Name for the new workspace:"));

    ImGui::InputText("##WorkspaceName", _name, IM_ARRAYSIZE(_name));

    if (ImGui::Button(_("OK"))) {
        callback(false, _name);
        visible = false;
    }

    ImGui::SameLine();
    if (ImGui::Button(_("Cancel"))) {
        callback(true, "");
        visible = false;
    }

    WINDOW_END_MODAL()
}
