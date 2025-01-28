
#include <iostream>
#include "../gui/editview.h"
#include "glad/glad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "imgui.h"
#include "../application/application.h"

#include <functional>

EditView::EditView(int width, int height) : width(width), height(height) {
    framebuffer = new Framebuffer;

    Application::GetInstance().framebuffer = framebuffer;
}

EditView::~EditView() { delete framebuffer; }

void EditView::Render() {
    Application::GetInstance().framebuffer->Bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.18f, 0.21f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImVec2 size = ImGui::GetContentRegionAvail();
    int newWidth = static_cast<int>(size.x);
    int newHeight = static_cast<int>(size.y);

    if (newWidth != framebuffer->width || newHeight != framebuffer->height) {
        framebuffer->Resize(newWidth, newHeight);
        Application::GetInstance().camera->UpdateViewport(newWidth, newHeight);
    }

    glViewport(0, 0, newWidth, newHeight);

    // Render Space
    for (const std::shared_ptr<Entity> entity : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        entity->Render();
    }

    Application::GetInstance().framebuffer->Unbind();

    Application& app = Application::GetInstance();
    if (app.editorMode == EditorMode::EDIT) {
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

    glBindTexture(GL_TEXTURE_2D, Application::GetInstance().framebuffer->GetTexture());

    ImGui::Image(Application::GetInstance().framebuffer->GetTexture(), ImVec2(size.x, size.y), ImVec2(0, 1), ImVec2(1, 0));

    imageMin = ImGui::GetWindowPos();
    imageMax = ImVec2(ImGui::GetWindowPos()[0] + ImGui::GetWindowSize()[0], ImGui::GetWindowPos()[1] + ImGui::GetWindowSize()[1]);
}

void EditView::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    Application::GetInstance().framebuffer->Resize(width, height);
}
