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
        auto newEntity = std::make_unique<Entity>(name, EntityFlags::NONE, Transform());

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

    if (ImGui::TreeNode(nodeLabel.c_str())) {
        if (ImGui::IsItemHovered()) {
            _selected = entity;
        }

        // Entity Transforms
        if (ImGui::InputFloat2(("Position##" + std::to_string(index)).c_str(), glm::value_ptr(entity->transform.pos))) {
            entity->setPosition(glm::vec2(entity->transform.pos[0], entity->transform.pos[1]));
        }
        if (ImGui::SliderFloat(("Rotation##" + std::to_string(index)).c_str(), &entity->transform.rotation, 0, 360)) {
            entity->setRotation((entity->transform.rotation));
        }
        if (ImGui::InputFloat2(("Scale##" + std::to_string(index)).c_str(), &entity->transform.scale[0])) {
            entity->setScale(glm::vec2(entity->transform.scale[0], entity->transform.scale[1]));
        }

        ImGui::Separator();

        ImGui::Image(ImTextureID(entity->renderable.GetTexture()), ImVec2(256, 256));

        ImGui::Text("Children:");

        int i = 0;
        for (auto& entity: entity->children) {
            displayEntity(entity.get(), i);
            i++;
        }


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
