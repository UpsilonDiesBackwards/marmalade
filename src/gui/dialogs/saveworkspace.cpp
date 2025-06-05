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
#include "../windowmanager.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::SaveWorkspaceDialog::Draw() {
    WINDOW_BEGIN_MODAL(GetName().c_str(), ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(_("Changes have been made to the workspace: %s."), workspaceName.c_str());

    ImGui::Text(_("Would you like to save these changes?"));

    DialogButtons::YesNoCancel([&] {
        CallbackData data{true};
        _dialog->Callback(true, &data);
        visible = false;
    }, [&] {
        CallbackData data{false};
        _dialog->Callback(true, &data);
        visible = false;
    }, [&] {
        CallbackData data{false};
        _dialog->Callback(false, &data);
        visible = false;
    });

    WINDOW_END_MODAL()
}

std::string Marmalade::GUI::SaveWorkspaceDialog::GetName() {
    return ICON_WITH_TEXT(ICON_CI_SAVE, _("Save Changes to Workspace"));
}

void Marmalade::GUI::SaveWorkspaceAsDialog::Draw() {
    WINDOW_BEGIN_MODAL(GetName().c_str(), ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(_("Name for the new workspace:"));

    ImGui::InputText("##WorkspaceName", _name, IM_ARRAYSIZE(_name));

    DialogButtons::OkCancel([&] {
        _dialog->Callback(true, _name);
        visible = false;
    }, [&] {
        _dialog->Callback(false, (void*) "");
        visible = false;
    });

    WINDOW_END_MODAL()
}

std::string Marmalade::GUI::SaveWorkspaceAsDialog::GetName() {
    std::string title;

    if (_type == DialogType_SAVE_AS) {
        title = ICON_WITH_TEXT(ICON_CI_SAVE, _("Save to New Workspace"));
    } else if (_type == DialogType_DUPLICATE) {
        title = ICON_WITH_TEXT(ICON_CI_SAVE, _("Duplicate Workspace"));
    }

    return title;
}
