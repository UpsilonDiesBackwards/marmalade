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

#include "details.h"

#include "../../application/plugins/interfaceimpl.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::Details::Draw() {
    WINDOW_BEGIN(ICON_CI_SEARCH " Details", ImGuiWindowFlags_None)

        if (!inspectedEntity) {
            visible = false;
            ImGui::End();
            return;
        }

        ImGui::Text(inspectedEntity->name.c_str());
        ImGui::Separator();

        for (int i = 0; i < inspectedEntity->componentManager.components.size(); ++i) {
            auto& comp = inspectedEntity->componentManager.components[i];
            ImGui::PushID(comp.get());

            bool open = ImGui::CollapsingHeader(comp->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            if (comp->isMutable) {
                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload("COMPONENT_REORDER", &i, sizeof(int));
                        ImGui::Text("Moving %s", comp->name.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
            }

            if (open) {
                comp->Display(inspectedEntity);

                if (comp->isMutable) {
                    float buttonWidth = ImGui::CalcTextSize(ICON_CI_TRASHCAN " Remove").x + ImGui::GetStyle().FramePadding.x * 2;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);
                    if (ImGui::Button(ICON_CI_TRASHCAN " Remove")) {
                        _isRemovingComponent = true;
                        _selectedComponent = comp.get();
                    }
                }
            }

            if (comp->isMutable) {
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_REORDER")) {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int srcIndex = *(const int*) payload->Data;
                        if (srcIndex != i) {
                            auto compToMove = inspectedEntity->componentManager.components[srcIndex];
                            inspectedEntity->componentManager.components.erase(
                                    inspectedEntity->componentManager.components.begin() + srcIndex);
                            inspectedEntity->componentManager.components.insert(
                                    inspectedEntity->componentManager.components.begin() + i, compToMove);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }


        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 125) * 0.5f);
        if (ImGui::Button(ICON_CI_PLUS " Add Component")) { _addComponentDialog.visible = true; }

        if (_isRemovingComponent) { ImGui::OpenPopup("Remove Component"); }

        ShowRemovePopup();
        _addComponentDialog.Draw();

    WINDOW_END()
}

/**
 * \brief Shows the modal dialog for removing a component from an entity
 */
void Marmalade::GUI::Details::ShowRemovePopup() {
    if (ImGui::BeginPopupModal("Remove Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove: %s?", _selectedComponent->name.c_str());

        if (ImGui::Button("Cancel")) {
            _selectedComponent = nullptr;
            _isRemovingComponent = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Confirm")) {
            inspectedEntity->componentManager.RemoveComponent(_selectedComponent);
            _selectedComponent = nullptr;
            _isRemovingComponent = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}