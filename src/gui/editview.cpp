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

#include "editview.h"

#include <glad/glad.h>

#include "../application/application.h"

#include <ImGuizmo.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <imgui.h>

#include <iostream>
#include <functional>

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
    for (const auto& entity : app.sceneManager.GetCurrentScene()->GetEntities()) {
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

    imageMin = ImGui::GetWindowPos();
    imageMax = ImVec2(imageMin.x + windowSize.x, imageMin.y + windowSize.y);

    framebuffer.position = windowPos;
}

void EditView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}

void EditView::RunInput() {
    if (ImGuizmo::IsUsing()) { return; }

    Application& app = Application::GetInstance();

    if (app.editorMode != EditorMode::EDIT) {
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

            app.camera->Move(-posX, -posY);
        }
    });

    app.input.BindScroll([&app](double xOffset, double yOffset) {
        app.camera->Zoom(yOffset);
    });

}
