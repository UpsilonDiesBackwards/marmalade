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

#include "scenehierarchy.h"

#include "../../application/application.h"
#include <../../../include/ecs/components/animation/animator.h>
#include "../windowmanager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <iostream>

static char newName[128] = "New Entity"; // New name to rename entity to

void SceneHierarchy::Show() {
    Application& app = Application::GetInstance();
    auto currentScene = app.sceneManager.GetCurrentScene();
    std::vector<std::shared_ptr<Entity>>& entities = currentScene->GetEntities();

    ImGui::SetNextWindowPos(ImVec2(220, 112), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(475, 760), ImGuiCond_FirstUseEver);

    ImGui::Begin("Hierarchy", nullptr);

    int index = 0;
    for (auto& entity: entities) {
        displayEntity(entity, index);
        index++;
    }

    showContextMenu();

    if (_isCreatingEntityChild) {
        if (!_selectedEntities.empty()) {
            _parent = _selectedEntities.front();
        }
        ImGui::OpenPopup("Create Entity");
    }

    if (_isRenaming) {
        ImGui::OpenPopup("Rename Entity");
    }

    if (_isDeleting) {
        ImGui::OpenPopup("Delete Entity");
    }

    showCreatePopup();
    showRenamePopup();
    showDeletePopup();

    if (!_selectedEntities.empty() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        DeselectEntity();
    }

    // Status Bar
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    float status_bar_height = 18.0f;

    ImVec2 p1(windowPos.x, windowPos.y + windowSize.y - status_bar_height);
    ImVec2 p2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

    draw_list->AddRectFilled(p1, p2, IM_COL32(30, 35, 38, 170));

    ImGui::SetCursorPos(ImVec2(4, ImGui::GetWindowHeight() - status_bar_height));

    if (_selectedEntities.empty()) {
        ImGui::Text("Entities: %zu", entities.size());
    } else {
        ImGui::Text("Selected Entities: %zu / %zu", _selectedEntities.size(), entities.size());
    }

    ImGui::End();
}

void SceneHierarchy::DeselectEntity() {
    _selectedEntities.clear();

    auto editorGui = Application::GetInstance().editorGUI;
    auto editView = Application::GetInstance().editView;

    editorGui->details.visible = false;
    editorGui->details.inspectedEntity = nullptr;
    editView->selectedEntity = nullptr;
}

void SceneHierarchy::createEntity(const std::string& name) {
    if (name.empty()) return;

    Application& app = Application::GetInstance();
    auto currentScene = app.sceneManager.GetCurrentScene();

    if (!name.empty()) {
        auto newEntity = std::make_unique<Entity>(name);

        if (auto parentPtr = _parent.lock()) {
            parentPtr->AddChild(parentPtr, std::move(newEntity));
        } else {
            Application::GetInstance().sceneManager.GetCurrentScene()->AddEntity(std::move(newEntity));
        }
    }
}

void SceneHierarchy::displayEntity(std::shared_ptr<Entity> entity, int index) {
    std::string nodeLabel = entity->name.empty() ? "New Entity" : entity->name;
    nodeLabel += "##" + std::to_string(index);

    entity->id = index;

    ImGuiTreeNodeFlags nodeFlags = entity->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;

    bool isSelected = false;
    for (const auto& selected : _selectedEntities) {
        if (auto locked = selected.lock()) {
            if (locked.get() == entity.get()) {
                isSelected = true;
                break;
            }
        }
    }

    if (isSelected) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(nodeLabel.c_str(), nodeFlags)) {
        if (ImGui::IsItemClicked()) {
            bool ctrlHeld = ImGui::GetIO().KeyCtrl;
            if (ctrlHeld) {
                auto it = std::find_if(_selectedEntities.begin(), _selectedEntities.end(),
                                       [&](const std::weak_ptr<Entity>& e) {
                                           return !e.expired() && e.lock().get() == entity.get();
                                       });

                if (it != _selectedEntities.end()) {
                    _selectedEntities.erase(it); // If already selected, then deselect
                } else {
                    _selectedEntities.push_back(entity); // Select entity
                }
            } else {
                _selectedEntities.clear();
                _selectedEntities.push_back(entity);

                Application::GetInstance().editorGUI->details.visible = true;
                Application::GetInstance().editorGUI->details.inspectedEntity = entity.get();
                Application::GetInstance().editView->selectedEntity = entity.get();

                auto animationPlayer = entity->componentManager.GetComponentOfType<Marmalade::ECS::AnimationPlayer>();
                if (animationPlayer && animationPlayer->driver) {
                        Marmalade::GUI::WindowManager::GetInstance().animationManager.driver = std::move(animationPlayer->driver);
                }
            }
        }

        int i = 0;
        for (auto& entity: entity->children) {
            displayEntity(entity, i);
            i++;
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::showCreatePopup() {
    if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        if (auto parentPtr = _parent.lock()) {
            ImGui::Text("Creating child entity for %s", parentPtr->name.c_str());
        }

        ImGui::InputText("Name", newName, IM_ARRAYSIZE(newName));

        if (ImGui::Button("Cancel")) {
            memset(newName, 0, sizeof(newName));
            _isCreatingEntityChild = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Create")) {
            createEntity(newName);

            memset(newName, 0, sizeof(newName));
            _isCreatingEntityChild = false;
            ImGui::CloseCurrentPopup();
        }

        if (strlen(newName) == 0) {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Invalid name! Name cannot be empty.");
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::showContextMenu() {
    if (ImGui::BeginPopupContextWindow("EntityRMBContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Entity")) {
            _isCreatingEntityChild = true;
        }

        if (!_selectedEntities.empty()) {
            if (ImGui::MenuItem("Rename")) {
                _isRenaming = true;
            }

            if (ImGui::MenuItem("Delete")) {
                _isDeleting = true;
            }
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::showRenamePopup() {
    if (ImGui::BeginPopupModal("Rename Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("New Name", newName, IM_ARRAYSIZE(newName));

        if (ImGui::Button("Cancel")) {
            _isRenaming = false;
            memset(newName, 0, sizeof(newName));
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Confirm")) {
            if (strlen(newName) > 0 && !_selectedEntities.empty()) {
                if (auto entity = _selectedEntities.front().lock()) {
                    entity->name = newName;
                }
                _isRenaming = false;
                memset(newName, 0, sizeof(newName));
                ImGui::CloseCurrentPopup();
            }
        }
        if (strlen(newName) == 0) {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Invalid name! Name cannot be empty.");
        }
        ImGui::EndPopup();
    }
}

void SceneHierarchy::showDeletePopup() {
    if (ImGui::BeginPopupModal("Delete Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (_selectedEntities.empty()) {
            _isDeleting = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        auto selected = _selectedEntities.front().lock();
        if (!selected) {
            _isDeleting = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }

        if (_selectedEntities.size() == 1 && selected) {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Are you sure you want to delete %s?", selected->name.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Are you sure you want to delete %zu entities?", _selectedEntities.size());
        }

        if (ImGui::Button("Cancel")) {
            _isDeleting = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Confirm")) {
            for (auto& weakEntity : _selectedEntities) {
                if (auto entity = weakEntity.lock()) {
                    if (auto parentPtr = entity->parent.lock()) {
                        parentPtr->RemoveChild(entity.get());
                    } else {
                        Application::GetInstance().sceneManager.GetCurrentScene()->RemoveEntity(entity.get());
                    }
                }
            }

            DeselectEntity();

            _isDeleting = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::SelectEntityByUuid(const std::string& uuid) {
    auto currentScene = Application::GetInstance().sceneManager.GetCurrentScene();
    auto& entities = currentScene->GetEntities(); // Reference, nice.

    for (auto& entity : entities) {
        if (entity->uuid == uuid) {
            _selectedEntities.clear();
            _selectedEntities.push_back(entity);

            auto editorGui = Application::GetInstance().editorGUI;
            auto editView = Application::GetInstance().editView;

            editorGui->details.visible = true;
            editorGui->details.inspectedEntity = entity.get();
            editView->selectedEntity = entity.get();
            return;
        }
    }
}
