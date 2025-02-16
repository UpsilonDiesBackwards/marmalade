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

#include "scenehierarchy.h"

#include "../../application/application.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <iostream>

static char newName[128] = "";// New name to rename entity to

void SceneHierarchy::Show() {
    Application& app = Application::GetInstance();
    auto currentScene = app.sceneManager.GetCurrentScene();
    std::vector<std::shared_ptr<Entity>>& entities = currentScene->GetEntities();

    ImGui::SetNextWindowPos(ImVec2(220, 112), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(475, 760), ImGuiCond_FirstUseEver);

    ImGui::Begin("Hierarchy", nullptr);

    int index = 0;
    for (auto& entity: entities) {
        displayEntity(entity.get(), index);
        index++;
    }

    showContextMenu();

    if (_isCreatingEntityChild) {
        _parent = _selected;
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

    ImGui::End();
}

void SceneHierarchy::createEntity(const std::string& name) {
    if (name.empty()) return;

    Application& app = Application::GetInstance();
    auto currentScene = app.sceneManager.GetCurrentScene();

    if (!name.empty()) {
        auto newEntity = std::make_unique<Entity>(name, EntityFlags::NONE);

        if (_parent) {
            _parent->AddChild(std::move(newEntity));
        } else {
            Application::GetInstance().sceneManager.GetCurrentScene()->AddEntity(std::move(newEntity));
        }
    }
}

void SceneHierarchy::displayEntity(Entity* entity, int index) {
    std::string nodeLabel = entity->name.empty() ? "Unnamed Entity" : entity->name;
    nodeLabel += "##" + std::to_string(index);

    entity->id = index;

    ImGuiTreeNodeFlags nodeFlags = entity->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;

    if (ImGui::TreeNodeEx(nodeLabel.c_str(), nodeFlags)) {
        if (ImGui::IsItemHovered()) {
            _selected = entity;
        }

        int i = 0;
        for (auto& entity: entity->children) {
            displayEntity(entity.get(), i);
            i++;
        }

        Application::GetInstance().editorGUI->details.visible = true;
        Application::GetInstance().editorGUI->details.inspectedEntity = _selected;

        ImGui::TreePop();
    }
}

void SceneHierarchy::showCreatePopup() {
    if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (_parent) {
            ImGui::Text("Creating child entity for %s", _parent->name.c_str());
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
        if (ImGui::MenuItem("Create Child Entity")) {
            _isCreatingEntityChild = true;
        }

        if (_selected) {
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
            if (strlen(newName) > 0) {
                _selected->name = newName;
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
        ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                           "Are you sure you want to delete %s ?", _selected->name.c_str());

        if (ImGui::Button("Cancel")) {
            _isDeleting = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Confirm")) {
            if (_selected->parent) {
                _selected->parent->RemoveChild(_selected);
            } else {
                Application::GetInstance().sceneManager.GetCurrentScene()->RemoveEntity(_selected);
            }
            _isDeleting = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
