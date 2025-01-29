#include <iostream>
#include "../gui/gameview.h"
#include <imgui.h>
#include <glad/glad.h>
#include "../../include/graphics/shader.h"
#include "../../include/graphics/renderable.h"
#include "../../include/archetypes/entity.h"
#include "../application/application.h"

/*
 * We will use a frame buffer to display a texture in an imgui window. this will allow us to have a detachable
 * view of our game.
 */

GameView::GameView(int width, int height) {
}

void GameView::Render() {
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

    if (app.editorMode == EditorMode::EDIT) {

    }

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
}

void GameView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}

void GameView::RunInput() {
}