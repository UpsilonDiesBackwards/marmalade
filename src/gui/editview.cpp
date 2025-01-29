
#include <iostream>
#include "../gui/editview.h"
#include "glad/glad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "imgui.h"
#include "../application/application.h"

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
    ImTextureID imTexture = Application::GetInstance().framebuffer->GetResolvedTexture();

    ImDrawList* drawlist = ImGui::GetWindowDrawList();

    ImVec2 windowPos = ImGui::GetCursorScreenPos();
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    drawlist->AddImage(imTexture,
                       ImVec2(windowPos.x, windowPos.y),
                       ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                       ImVec2(0, 1), ImVec2(1, 0));

    imageMin = ImGui::GetWindowPos();
    imageMax = ImVec2(imageMin.x + windowSize.x, imageMin.y + windowSize.y);
}

void EditView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}

void EditView::RunInput() {
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
