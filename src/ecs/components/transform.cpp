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

#include <ecs/components/transform.h>

#include "../../application/application.h"

#include <scene/entity.h>

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

void Marmalade::ECS::Transform::Display(Entity* entity) {
        Application& app = Application::GetInstance();

        ImGui::Text("%s", name.c_str());

        static ImGuizmo::OPERATION currentGuizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE currentGuizmoMode(ImGuizmo::WORLD);

        if (ImGui::IsKeyPressed(ImGuiKey_Q)) { currentGuizmoOperation = ImGuizmo::TRANSLATE; }
        if (ImGui::IsKeyPressed(ImGuiKey_W)) { currentGuizmoOperation = ImGuizmo::ROTATE; }
        if (ImGui::IsKeyPressed(ImGuiKey_E)) { currentGuizmoOperation = ImGuizmo::SCALE; }

        if (ImGuizmo::IsUsing()) {
            glm::vec3 translation, scale, rotation;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(entity->transform->modelMatrix),
                                                  glm::value_ptr(translation),
                                                  glm::value_ptr(rotation),
                                                  glm::value_ptr(scale));

            entity->setPosition(glm::vec2(translation.x, translation.y));
            entity->setRotation(rotation.z);
            entity->setScale(glm::vec2(scale.x, scale.y));

            ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(entity->transform->modelMatrix),
                                                    glm::value_ptr(translation),
                                                    glm::value_ptr(rotation),
                                                    glm::value_ptr(scale));
        }

        ImGui::Text(ImGuizmo::IsOver()?"Over gizmo":"");
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "not");

        // Transform
        if (ImGui::DragFloat2(("Position##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->pos), 0.1f)) {
            entity->setPosition(glm::vec2(entity->transform->pos[0], entity->transform->pos[1]));
        }
        if (ImGui::SliderFloat(("Rotation##" + std::to_string(entity->id)).c_str(), &entity->transform->rotation, 0, 360)) {
            entity->setRotation(entity->transform->rotation);
        }
        if (ImGui::DragFloat2(("Scale##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->scale), 0.1f)) {
            entity->setScale(glm::vec2(entity->transform->scale[0], entity->transform->scale[1]));
        }

        if (currentGuizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", currentGuizmoMode == ImGuizmo::LOCAL)) {
                currentGuizmoMode = ImGuizmo::LOCAL;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("World", currentGuizmoMode == ImGuizmo::WORLD)) {
                currentGuizmoMode = ImGuizmo::WORLD;
            }
        }

        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

        ImGuizmo::SetRect(app.framebuffer->position.x, app.framebuffer->position.y,
                          app.framebuffer->width, app.framebuffer->height);

        ImGuizmo::Manipulate(glm::value_ptr(app.camera->GetView()),
                             glm::value_ptr(app.camera->GetProjection()),
                             currentGuizmoOperation, currentGuizmoMode,
                             glm::value_ptr(entity->transform->modelMatrix));
}

void Marmalade::ECS::Transform::Apply(Entity* entity) {

}
