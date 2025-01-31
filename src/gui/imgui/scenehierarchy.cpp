#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../gui/imgui/scenehierarchy.h"
#include "../../application/application.h"
#include <imgui.h>

void SceneHierarchy::Show() {
    Application& app = Application::GetInstance();
    auto currentScene = app.sceneManager.GetCurrentScene();
    std::vector<std::shared_ptr<Entity>>& entities = currentScene->GetEntities();

    ImGui::SetNextWindowPos(ImVec2(220, 112), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(475, 760), ImGuiCond_FirstUseEver);

    // Window / Popup control
    bool isCreatingEntity = false;
    bool isCreatingEntityChild = false;
    bool isRenamingEntity = false;
    bool isDeletingEntity = false;

    static char newName[128] = ""; // New name to rename entity to
    static int renameIndex = -1; // Index of entity to rename

    ImGui::Begin("Hierarchy", nullptr);

    if (ImGui::BeginPopupContextWindow("GenericContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Entity")) {
            isCreatingEntity = true;
        }
        ImGui::EndPopup();
    }

    // Entity create

    if (isCreatingEntity) { ImGui::OpenPopup("Create Entity##"); }

    if (ImGui::BeginPopupModal("Create Entity##", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Name", newName, IM_ARRAYSIZE(newName));

        if (ImGui::Button("Cancel")) {
            memset(newName, 0, sizeof(newName));
            isCreatingEntity = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create")) {
            std::string name = std::string(newName);

            if (!name.empty()) {
                Transform defaultTransform;
                currentScene->AddEntity(std::make_shared<Entity>(newName, EntityFlags::RENDERABLE, defaultTransform));

                memset(newName, 0, sizeof(newName));
                isCreatingEntity = false;
                ImGui::CloseCurrentPopup();
            }
        }

        if (strlen(newName) == 0) {
            ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                               "Invalid name! Name cannot be empty.");
        }

        ImGui::EndPopup();
    }

    for (size_t i = 0; i < entities.size(); ++i) { // Display entities in ImGui tree
        auto &entity = entities[i];

        std::string nodeLabel = entity->name.empty() ? "Unnamed Entity" : entity->name;
        nodeLabel += "##" + std::to_string(i);

        if (ImGui::TreeNode(entity->name.c_str())) {
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { // Right-click to open entity context menu
                ImGui::OpenPopup("EntityRMBContextMenu");
            }

            // Entity right mouse button context menu
            if (ImGui::BeginPopup("EntityRMBContextMenu")) {
                if (ImGui::MenuItem("Create Child Entity")) {
                    isCreatingEntityChild = true;
                }

                if (ImGui::MenuItem("Rename")) {
                    renameIndex = i;
                    strncpy(newName, entity->name.c_str(), sizeof(newName));
                    newName[sizeof(newName) - 1] = '\0';
                    isRenamingEntity = true;
                }

                if (ImGui::MenuItem("Delete")) {
                    isDeletingEntity = true;
                }
                ImGui::EndPopup();
            }

            // Entity child create

            if (isCreatingEntityChild) { ImGui::OpenPopup(("Create Child##" + std::to_string(i)).c_str()); }

            if (ImGui::BeginPopupModal(("Create Child##" + std::to_string(i)).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::EndPopup();
            }

            // Entity rename

            if (isRenamingEntity) { ImGui::OpenPopup(("Rename Entity##" + std::to_string(i)).c_str()); }

            if (ImGui::BeginPopupModal(("Rename Entity##" + std::to_string(i)).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                if (renameIndex == static_cast<int>(i)) {
                    ImGui::InputText("New Name", newName, IM_ARRAYSIZE(newName));

                    if (ImGui::Button("Cancel")) {
                        renameIndex = -1;
                        isRenamingEntity = false;
                        memset(newName, 0, sizeof(newName));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Confirm")) {
                        if (strlen(newName) > 0) {
                            entity->name = newName;
                            renameIndex = -1;
                            isRenamingEntity = false;
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

            // Entity delete

            if (isDeletingEntity) { ImGui::OpenPopup(("Delete Entity##")); }

            if (ImGui::BeginPopupModal("Delete Entity##", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                                   "Are you sure you want to delete %s ?", entity->name.c_str());

                if (ImGui::Button("Cancel")) {
                    isDeletingEntity = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Confirm")) {
                    currentScene->RemoveEntity(entity);
                    isDeletingEntity = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            // Entity Transforms
            if (ImGui::InputFloat2("Position", glm::value_ptr(entity->transform.pos))) {
                entity->setPosition(glm::vec2(entity->transform.pos[0], entity->transform.pos[1]));
            }
            if (ImGui::SliderFloat("Rotation", &entity->transform.rotation, 0, 360)) {
                entity->setRotation((entity->transform.rotation));
            }
            if (ImGui::InputFloat2("Scale", &entity->transform.scale[0])) {
                entity->setScale(glm::vec2(entity->transform.scale[0], entity->transform.scale[1]));
            }

            ImGui::Separator();

            ImGui::Image(ImTextureID(entity->renderable.GetTexture()), ImVec2(256, 256));

            ImGui::TreePop();
        }
    }
    ImGui::End();
}
