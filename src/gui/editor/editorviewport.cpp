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

#include "editorviewport.h"

#include "../../application/application.h"
#include "ecs/components/lighting2d/light2d.h"

#include <ecs/components/physics2d/colliderbase.h>

#include <glad/glad.h>

#include <imgui.h>

#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

EditView::EditView(int width, int height) : width(width), height(height) {
    Application::GetInstance().framebuffer = &framebuffer;
}

EditView::~EditView() {
}

void EditView::Render() {
    Application& app = Application::GetInstance();

    app.framebuffer->Bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.18f, 0.21f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImVec2 size = ImGui::GetContentRegionAvail();
    int newWidth = static_cast<int>(size.x);
    int newHeight = static_cast<int>(size.y);

    if (newWidth > 0 && newHeight > 0 && (newWidth != framebuffer.width || newHeight != framebuffer.height)) {
        framebuffer.Resize(newWidth, newHeight);
        app.camera->UpdateViewport(newWidth, newHeight);
        glViewport(0, 0, newWidth, newHeight);
    }

    // Render Space
    for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
        entity->Render();
    }

    Application::GetInstance().framebuffer->Unbind();

    Application::GetInstance().framebuffer->Resolve();
    ImTextureID imTexture = Application::GetInstance().framebuffer->GetTexture();

    ImDrawList* drawlist = ImGui::GetWindowDrawList();

    ImVec2 windowPos = ImGui::GetCursorScreenPos();
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    drawlist->AddImage(imTexture,
                       ImVec2(windowPos.x, windowPos.y),
                       ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                       ImVec2(0, 1), ImVec2(1, 0));

    for (const auto& entity : app.sceneManager.GetCurrentScene()->GetEntities()) {
        auto lightComponent = entity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();
        if (lightComponent) {
            lightComponent->ShowBounds(entity.get());
        }
    }

    ImGui::SetCursorScreenPos(windowPos);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));

    ImVec2 toolbarSize = ImVec2(windowSize.x, 30.0f);

    if (ImGui::BeginChild("EditViewTopBar", toolbarSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        if (_currentGuizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("World", _currentGuizmoMode == ImGuizmo::WORLD)) {
                _currentGuizmoMode = ImGuizmo::WORLD;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Local", _currentGuizmoMode == ImGuizmo::LOCAL)) {
                _currentGuizmoMode = ImGuizmo::LOCAL;
            }
        }

        ImGui::SameLine();

        static int currentRenderMode = 1;
        const char* renderModes[] = { "Lit", "Unlit", "Wireframe" };

        ImGui::SetNextItemWidth(95);
        if (ImGui::Combo("Render Mode", &currentRenderMode, renderModes, IM_ARRAYSIZE(renderModes))) {
            for (const auto& entity : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
                entity->renderable.renderMode = static_cast<Renderable::RenderMode>(currentRenderMode);
                entity->renderable.ApplyRenderMode();
            }
        }

        ImGui::SameLine();

        ImGui::Text("Light count: %zu", Application::GetInstance().sceneManager.GetCurrentScene()->GetLights().size());
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // FIXME: Frame buffer image doesn't take into account tab bar, and bottom bar.
    // ...    Hence leaves a dead area in the bottom of the window when scrolling.
    imageMin = ImGui::GetWindowPos();
    imageMax = ImVec2(imageMin.x + windowSize.x, imageMin.y + windowSize.y);

    framebuffer.position = windowPos;

    ShowEditorUIGuizmos();
}

void EditView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}

void EditView::RunInput() {
    if (ImGuizmo::IsUsing() ||
        ImGui::IsAnyItemActive() ||
        ImGui::IsAnyItemFocused() ||
        ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup)) {

        return;
    }

    Application& app = Application::GetInstance();

    if (app.editorMode != EditorMode::EditorMode_EDIT) {
        app.input.UnbindMouseButton(GLFW_MOUSE_BUTTON_RIGHT, MOUSE_PRESSED);
        app.input.UnbindScroll();
        return;
    }

    app.input.BindMouseButton(GLFW_MOUSE_BUTTON_RIGHT, MOUSE_PRESSED,
                              [&app, this]() {
                                  ImVec2 mousePos = ImGui::GetMousePos();

                                  if (mousePos.x >= imageMin.x && mousePos.x <= imageMax.x &&
                                      mousePos.y >= imageMin.y && mousePos.y <= imageMax.y) {

                                      float posX = app.inputManager.getMouseDeltaX();
                                      float posY = app.inputManager.getMouseDeltaY();

                                      app.camera->Move(-posX, posY);
                                  }
                              });

    // Select entities by holding ctrl
    app.input.BindKey(GLFW_KEY_LEFT_CONTROL, KEY_DOWN, [&app]() {
        ImVec2 mouseCoords = ImVec2(app.inputManager.getMouseX(), app.inputManager.getMouseY());
        auto worldCoords = EditorViews::ScreenToWorldSpace(mouseCoords);

        for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
            auto entityPos = entity->getPosition();
            auto entityScale = entity->getScale();

            glm::vec2 maxBounds = entityPos + entityScale;

            bool withinBounds(worldCoords.x >= entityPos.x && worldCoords.x <= maxBounds.x &&
                              worldCoords.y >= entityPos.y && worldCoords.y <= maxBounds.y);
            if (withinBounds) {
                auto screenMin = EditorViews::WorldToScreenSpace(entity->getPosition());
                auto screenMax = EditorViews::WorldToScreenSpace(entity->getPosition() + entity->getScale());

                ImGui::GetForegroundDrawList(ImGui::GetMainViewport())->AddRect(screenMin, screenMax, ImGui::GetColorU32(IM_COL32(255, 255, 255, 255)), 0.0f, ImDrawFlags_None, 1.0f);

                if (app.inputManager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
                    app.editorGUI->sceneHierarchy.SelectEntityByUuid(entity->uuid);
                }
            }
        }
    });

    app.input.BindScroll([&app, this](double xOffset, double yOffset) {
        auto mousePos = ImGui::GetMousePos();

        // Check if mouse is in bounds
        bool withinBounds = (mousePos.x >= imageMin.x && mousePos.x <= imageMax.x) &&
                            (mousePos.y >= imageMin.y && mousePos.y <= imageMax.y);

        if (withinBounds) app.camera->Zoom(yOffset);
    });
}

void EditView::ShowEditorUIGuizmos() {
    if (selectedEntity) {
        ShowGizmo();
        ShowColliderBounds();
    }
}

void EditView::ShowGizmo() {
    Application& app = Application::GetInstance();

    auto transform = selectedEntity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>();
    if (!transform) return;

    if (!(ImGuizmo::IsUsing() ||
          ImGui::IsAnyItemActive() ||
          ImGui::IsAnyItemFocused() ||
          ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup))) {

        if (ImGui::IsKeyPressed(ImGuiKey_Q)) { _currentGuizmoOperation = ImGuizmo::TRANSLATE; }
        if (ImGui::IsKeyPressed(ImGuiKey_W)) { _currentGuizmoOperation = ImGuizmo::ROTATE; }
        if (ImGui::IsKeyPressed(ImGuiKey_E)) { _currentGuizmoOperation = ImGuizmo::SCALE; }
    }

    if (ImGuizmo::IsUsing()) {
        glm::vec3 translation, scale, rotation;
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform->modelMatrix),
                                              glm::value_ptr(translation),
                                              glm::value_ptr(rotation),
                                              glm::value_ptr(scale));

        selectedEntity->setPosition(glm::vec2(translation.x, translation.y));
        selectedEntity->setRotation(rotation.z);
        selectedEntity->setScale(glm::vec2(scale.x, scale.y));

        selectedEntity->UpdateModelMatrix();

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(translation),
                                                glm::value_ptr(rotation),
                                                glm::value_ptr(scale),
                                                glm::value_ptr(transform->modelMatrix));
    }

    ImGuizmo::SetOrthographic(true);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(app.framebuffer->position.x, app.framebuffer->position.y,
                      app.framebuffer->width, app.framebuffer->height);

    ImGuizmo::Manipulate(glm::value_ptr(app.camera->GetView()),
                         glm::value_ptr(app.camera->GetProjection()),
                         _currentGuizmoOperation, _currentGuizmoMode,
                         glm::value_ptr(transform->modelMatrix));
}

void EditView::ShowColliderBounds() {
    auto comp = selectedEntity->componentManager.GetComponentOfType<Marmalade::ECS::ColliderBase>();

    if (!comp || !comp->showingBounds) { return; }// Collider component does not exist, or not showing bounds. Do not continue

    auto* transform = selectedEntity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>();

    comp->ShowBounds(selectedEntity->getPosition(), *transform);
}

void EditView::ShowLightBounds() {
    auto lightComp = selectedEntity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();

    if (!lightComp || !lightComp->showingBounds) {
        return;
    }

    lightComp->ShowBounds(selectedEntity);
}
