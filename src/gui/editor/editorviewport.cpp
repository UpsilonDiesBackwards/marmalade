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
#include "ecs/components/light2d/light2d.h"

//#include <ecs/components/physics2d/colliderbase.h>

#include <glad/glad.h>

#include <imgui.h>

#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include "ecs/components/rendering/tilemap.h"
#include "ecs/components/physics/2d/rigidbody2d.h"

int EditView::currentRenderMode = 1;

/**
 * \brief Constructor of the editor viewport. Assigns the initial framebuffer
 * \param width Starting width of the viewport
 * \param height Starting height of the viewport
 */
EditView::EditView(int width, int height) : width(width), height(height) { Application::GetInstance().framebuffer = &framebuffer; }

EditView::~EditView() {}

/**
 * \brief Sets the framebuffer and shows editor viewport toolbar which includes drop down for render mode and guizmo mode toggles
 */
void EditView::Render() {
    Application& app = Application::GetInstance();

    app.framebuffer->Bind();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
    for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) { entity->Render(); }

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

    for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
        auto tileMap = entity->componentManager.GetComponentOfType<Marmalade::ECS::TileMap>();
        if (tileMap) { tileMap->RenderGUIGrid(entity.get()); }
    }

    for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
        auto lightComponent = entity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();
        if (lightComponent) { lightComponent->ShowBounds(entity.get()); }
    }

    ImGui::SetCursorScreenPos(windowPos);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));

    ImVec2 toolbarSize = ImVec2(windowSize.x, 30.0f);

    if (ImGui::BeginChild("EditViewTopBar", toolbarSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        if (_currentGuizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("World", _currentGuizmoMode == ImGuizmo::WORLD)) { _currentGuizmoMode = ImGuizmo::WORLD; }
            ImGui::SameLine();
            if (ImGui::RadioButton("Local", _currentGuizmoMode == ImGuizmo::LOCAL)) { _currentGuizmoMode = ImGuizmo::LOCAL; }
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(95);
        if (ImGui::Combo("Render Mode", &currentRenderMode, renderModes, IM_ARRAYSIZE(renderModes))) {
            for (const auto& entity: Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
                entity->renderable.renderMode = static_cast<Renderable::RenderMode>(currentRenderMode);
                entity->renderable.ApplyRenderMode();
            }
        }

        ImGui::SameLine();

        static const char* cameraModes[] = {"Orthographic", "Perspective", "Top", "Bottom", "Left", "Right"};
        static int currentCameraMode = 0;

        Camera* cam = Application::GetInstance().camera;
        switch (cam->GetViewportMode()) {
            case Camera::ViewportMode_ORTHOGRAPHIC: currentCameraMode = 0; break;
            case Camera::ViewportMode_PERSPECTIVE:  currentCameraMode = 1; break;
            case Camera::ViewportMode_TOP:          currentCameraMode = 2; break;
            case Camera::ViewportMode_BOTTOM:       currentCameraMode = 3; break;
            case Camera::ViewportMode_LEFT:         currentCameraMode = 4; break;
            case Camera::ViewportMode_RIGHT:        currentCameraMode = 5; break;
            default:                                currentCameraMode = 0; break;
        }

        ImGui::SetNextItemWidth(110);

        Camera::ViewportMode modes[] = {
                Camera::ViewportMode_ORTHOGRAPHIC,
                Camera::ViewportMode_PERSPECTIVE,
                Camera::ViewportMode_TOP,
                Camera::ViewportMode_BOTTOM,
                Camera::ViewportMode_LEFT,
                Camera::ViewportMode_RIGHT
        };

        if (ImGui::Combo("Camera", &currentCameraMode, cameraModes, IM_ARRAYSIZE(cameraModes))) { cam->SetViewportMode(modes[currentCameraMode]); }
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
        ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup)) { return; }

    Application& app = Application::GetInstance();

    Camera::ViewportMode mode = GET_APP.camera->GetViewportMode();
    switch (mode) {
        case Camera::ViewportMode_PERSPECTIVE:
            ViewportPerspectiveInput();
            break;
        case Camera::ViewportMode_TOP:
        case Camera::ViewportMode_BOTTOM:
        case Camera::ViewportMode_LEFT:
        case Camera::ViewportMode_RIGHT:
        case Camera::ViewportMode_ORTHOGRAPHIC:
            ViewportOrthographicInput();
            break;
        default:
            break;
    }
}

void EditView::ViewportOrthographicInput() {
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

    app.input.BindKey(GLFW_KEY_LEFT_CONTROL, KEY_DOWN, [&app]() {
        ImVec2 mouseCoords = ImVec2(app.inputManager.getMouseX(), app.inputManager.getMouseY());
        auto worldCoords = EditorViews::ScreenToWorldSpace(mouseCoords);

        for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
            auto entityPos = entity->getPosition();
            auto entityScale = entity->getScale();

            glm::vec2 minBounds = entityPos - entityScale * 0.5f;
            glm::vec2 maxBounds = entityPos + entityScale * 0.5f;

            bool withinBounds = (worldCoords.x >= minBounds.x && worldCoords.x <= maxBounds.x &&
                                 worldCoords.y >= minBounds.y && worldCoords.y <= maxBounds.y);
            if (withinBounds) {
                auto screenMin = EditorViews::WorldToScreenSpace(minBounds);
                auto screenMax = EditorViews::WorldToScreenSpace(maxBounds);

                ImGui::GetForegroundDrawList(ImGui::GetMainViewport())->AddRect(
                        screenMin, screenMax,
                        ImGui::GetColorU32(IM_COL32(249, 226, 175, 255)),
                        0.0f, ImDrawFlags_None, 2.0f);

                if (app.inputManager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { app.editorGUI->sceneHierarchy.SelectEntityByUuid(entity->uuid); }
            }
        }
    });

    app.input.BindScroll([&app, this](double xOffset, double yOffset) {
        ImVec2 mousePos = ImGui::GetMousePos();

        if (mousePos.x < imageMin.x || mousePos.x > imageMax.x ||
            mousePos.y < imageMin.y || mousePos.y > imageMax.y) { return; }

        glm::vec2 worldBefore = EditorViews::ScreenToWorldSpace(mousePos);
        app.camera->Zoom(yOffset);
        glm::vec2 worldAfter = EditorViews::ScreenToWorldSpace(mousePos);

        glm::vec2 delta = worldBefore - worldAfter;
        app.camera->Move(delta.x, delta.y, true);
    });
}

void EditView::ViewportPerspectiveInput() {
    Application& app = Application::GetInstance();
    static float moveSpeed = 5.0f;
    static float mouseSensitivity = 0.1f;

    static bool rotating = false;

    if (app.inputManager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        rotating = true;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    } else {
        rotating = false;
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }

    if (rotating) {
        float dx = app.inputManager.getMouseDeltaX();
        float dy = app.inputManager.getMouseDeltaY();

        Camera* cam = app.camera;
        cam->yaw += dx * mouseSensitivity;
        cam->pitch -= dy * mouseSensitivity;

        cam->pitch = glm::clamp(cam->pitch, -89.0f, 89.0f);
        cam->UpdateVectors();
    }

    Camera* cam = app.camera;
    glm::vec3& pos = cam->position3D;

    float deltaTime = app.time.GetDeltaTime();
    float speed = moveSpeed * deltaTime;

    if (app.inputManager.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) speed *= 2.0f;

    if (app.inputManager.isKeyDown(GLFW_KEY_W)) pos += cam->front * speed;
    if (app.inputManager.isKeyDown(GLFW_KEY_S)) pos -= cam->front * speed;
    if (app.inputManager.isKeyDown(GLFW_KEY_A)) pos -= cam->right * speed;
    if (app.inputManager.isKeyDown(GLFW_KEY_D)) pos += cam->right * speed;
    if (app.inputManager.isKeyDown(GLFW_KEY_Q)) pos -= cam->up * speed;
    if (app.inputManager.isKeyDown(GLFW_KEY_E)) pos += cam->up * speed;

    app.input.BindKey(GLFW_KEY_LEFT_CONTROL, KEY_DOWN, [&app]() {
        ImVec2 mouseCoords = ImVec2(app.inputManager.getMouseX(), app.inputManager.getMouseY());
        auto worldCoords = EditorViews::ScreenToWorldSpace(mouseCoords);

        for (const auto& entity: app.sceneManager.GetCurrentScene()->GetEntities()) {
            auto entityPos = entity->getPosition();
            auto entityScale = entity->getScale();

            glm::vec2 minBounds = entityPos - entityScale * 0.5f;
            glm::vec2 maxBounds = entityPos + entityScale * 0.5f;

            bool withinBounds = (worldCoords.x >= minBounds.x && worldCoords.x <= maxBounds.x &&
                                 worldCoords.y >= minBounds.y && worldCoords.y <= maxBounds.y);
            if (withinBounds) {
                auto screenMin = EditorViews::WorldToScreenSpace(minBounds);
                auto screenMax = EditorViews::WorldToScreenSpace(maxBounds);

                ImGui::GetForegroundDrawList(ImGui::GetMainViewport())->AddRect(
                        screenMin, screenMax,
                        ImGui::GetColorU32(IM_COL32(249, 226, 175, 255)),
                        0.0f, ImDrawFlags_None, 2.0f);

                if (app.inputManager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { app.editorGUI->sceneHierarchy.SelectEntityByUuid(entity->uuid); }
            }
        }
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

        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            if (ImGui::IsKeyPressed(ImGuiKey_Q)) { _currentGuizmoOperation = ImGuizmo::TRANSLATE; }
            if (ImGui::IsKeyPressed(ImGuiKey_W)) { _currentGuizmoOperation = ImGuizmo::ROTATE; }
            if (ImGui::IsKeyPressed(ImGuiKey_E)) { _currentGuizmoOperation = ImGuizmo::SCALE; }
        }
    }

    if (ImGuizmo::IsUsing()) {
        glm::vec3 translation, scale, rotation;
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform->modelMatrix),
                                              glm::value_ptr(translation),
                                              glm::value_ptr(rotation),
                                              glm::value_ptr(scale));

        selectedEntity->setPosition(translation);
        selectedEntity->setRotation(rotation);
        selectedEntity->setScale(scale);

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
    for (auto& entity: GET_APP.sceneManager.GetCurrentScene()->GetEntities()) {
        auto rb = entity->componentManager.GetComponentOfType<Marmalade::ECS::Rigidbody2D>();
        if (!rb) continue;

        const auto& collider = rb->body.collider;
        if (collider.shape) {
            glm::vec3 pos = entity->getPosition();
            glm::quat rot = entity->getRotation();
            if (auto transformPtr = entity->componentManager.GetComponentOfType<Marmalade::ECS::Transform>()) { collider.shape->ShowBounds({pos.x, pos.y}, *transformPtr); }
        }
    }
}

void EditView::ShowLightBounds() {
    auto lightComp = selectedEntity->componentManager.GetComponentOfType<Marmalade::ECS::Light2D>();

    if (!lightComp || !lightComp->showingBounds) { return; }

    lightComp->ShowBounds(selectedEntity);
}