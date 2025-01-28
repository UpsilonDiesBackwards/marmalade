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
    Application::GetInstance().framebuffer->Bind();

    float aRatio = static_cast<float>(width) / static_cast<float>(height);

    glViewport(0, 0, width, height);
    glClearColor(0.49f, 0.73f, 0.70f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDER IN THE GAME VIEW
    for (const std::shared_ptr<Entity> entity : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        entity->Render();
    }

    Application::GetInstance().framebuffer->Unbind();

    ImVec2 winSize = ImGui::GetContentRegionAvail();
    float wRatio = winSize.x / winSize.y;

    if (winSize.x != width || winSize.y != height) {
        width = static_cast<int>(winSize.x);
        height = static_cast<int>(winSize.y);
        Resize(width, height);
    }

    // Retain Aspect Ratio
    if (wRatio > aRatio) {
        width = static_cast<int>(winSize.y * aRatio);
        height = static_cast<int>(winSize.y);
    } else {
        width = static_cast<int>(winSize.x);
        height = static_cast<int>(winSize.x / aRatio);
    }

    ImGui::Image(Application::GetInstance().framebuffer->GetTexture(), ImVec2(winSize.x, winSize.y), ImVec2(0, 1), ImVec2(1, 0));
}

void GameView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}
