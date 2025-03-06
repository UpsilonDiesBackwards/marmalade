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

#include "imgui.h"

#include "IconsCodicons.h"

void Marmalade::GUI::Details::Draw() {
    ImGui::Begin(ICON_CI_SEARCH " Details", &visible);

    if (inspectedEntity) {
        ImGui::Text(inspectedEntity->name.c_str());
        ImGui::Separator();

        for (auto& comp: inspectedEntity->componentManager.components) {
            ImGui::PushID(comp.get());

            comp->Display(inspectedEntity);

            if (comp->isMutable) { // If entity is mutable then allow it to be removed
                if (ImGui::Button(ICON_CI_TRASHCAN " Remove")) {
                    _isRemovingComponent = true;
                    _selectedComponent = comp.get();
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 125) * 0.5f);
        if (ImGui::Button(ICON_CI_PLUS " Add Component")) {
            _isAddingComponent = true;
        }
    }

    if (_isRemovingComponent) {
        ImGui::OpenPopup("Remove Component");
    }

    if (_isAddingComponent) {
        ImGui::OpenPopup("Add Component");
    }

    ShowRemovePopup();
    ShowAddPopup();

    ImGui::End();
}

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

void Marmalade::GUI::Details::ShowAddPopup() {
    if (ImGui::BeginPopupModal("Add Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Choose component: ");

        for (const auto& [name, factory]: Marmalade::ECS::ComponentRegistry::Instance().GetRegisteredComponents()) {
            if (ImGui::Button(name.c_str())) {
                inspectedEntity->componentManager.AddComponent(factory->Create());// Add component

                _isAddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
        }

        if (ImGui::Button("Close")) {
            _isAddingComponent = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
