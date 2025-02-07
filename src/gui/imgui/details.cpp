
#include "details.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

void Marmalade::GUI::Details::Draw() {
    ImGui::Begin(ICON_CI_SEARCH " Details", &visible );

    if (inspectedEntity) {
        ImGui::Text(inspectedEntity->name.c_str());
        ImGui::Separator();

        for (auto& comp : inspectedEntity->componentManager.components) {
            ImGui::PushID(comp.get());

            comp->Display(inspectedEntity);

            if (ImGui::Button(ICON_CI_TRASHCAN " Remove")) {
                _isRemovingComponent = true;
                _selectedComponent = comp.get();
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

        for (const auto& [type, name] : Component::GetRegisteredComponents()) {
            if (ImGui::Button(name.c_str())) {
                // TODO: List registered components
//                inspectedEntity->componentManager.AddComponent(); // Add component
            }
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
