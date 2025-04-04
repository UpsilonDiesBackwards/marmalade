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

#include "editor.h"

#include "../../application/application.h"

void EditorViews::Show() {
    Application& application = Application::GetInstance();

    ImGui::SetNextWindowPos(ImVec2(763, 107), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1011, 749), ImGuiCond_FirstUseEver);
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("Views")) {
        if (ImGui::BeginTabItem("Edit")) {
            Application::GetInstance().input.ClearAllInputEvents();
            Application::GetInstance().editView->RunInput();
            application.editView->Render();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Game")) {
            Application::GetInstance().input.ClearAllInputEvents();
            Application::GetInstance().gameView->RunInput();
            application.gameView->Render();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // Status Bar
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    float status_bar_height = 18.0f;

    ImVec2 p1(windowPos.x, windowPos.y + windowSize.y - status_bar_height);
    ImVec2 p2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

    draw_list->AddRectFilled(p1, p2, IM_COL32(39, 46, 51, 255));

    ImGui::SetCursorPos(ImVec2(4, ImGui::GetWindowHeight() - status_bar_height));
    ImGui::Text("Cursor: (X: %.1f, Y: %.1f ) (%.1f) | Play State: %s | Editor Mode: %s | Scene: %s",
                (application.camera->GetPosition()[0]), application.camera->GetPosition()[1],
                application.camera->GetZoom(),
                (application.playState == PlayState::PlayState_PLAY ? "Playing" : "Stopped"),
                (application.editorMode == EditorMode::EditorMode_EDIT ? "Edit" : "Game"),
                (application.sceneManager.GetCurrentScene()->GetName().c_str()));

    ImGui::End();
}

ImVec2 EditorViews::WorldToScreenSpace(const Marmalade::Mathematics::Vec2& world) {
    Application& app = Application::GetInstance();

    Marmalade::Mathematics::Vec2 framebufferSize(static_cast<float>(app.framebuffer->width), static_cast<float>(app.framebuffer->height));
    Marmalade::Mathematics::Vec2 framebufferPos(static_cast<float>(app.framebuffer->position.x), static_cast<float>(app.framebuffer->position.y));

    Marmalade::Mathematics::Vec4 worldPos(world[0], world[1], 0.0f, 1.0f);

    Marmalade::Mathematics::Vec4 clipSpace = app.camera->GetProjection() * app.camera->GetView() * worldPos;

    if (clipSpace[3] != 0.0f) {
        clipSpace /= clipSpace[3];
    }

    float screenX = (clipSpace[0] * 0.5f + 0.5f) * framebufferSize[0];
    float screenY = (1.0f - (clipSpace[1] * 0.5f + 0.5f)) * framebufferSize[1]; // Flip Y

    return {screenX + framebufferPos[0], screenY + framebufferPos[1]};
}

glm::vec2 EditorViews::ScreenToWorldSpace(const ImVec2& screen) {
    Application& app = Application::GetInstance();

    glm::vec2 framebufferSize = glm::vec2(app.framebuffer->width, app.framebuffer->height);
    glm::vec2 framebufferPos = glm::vec2(app.framebuffer->position.x, app.framebuffer->position.y);

    float ndcX = ((screen.x - framebufferPos.x) / framebufferSize.x) * 2.0f - 1.0f;
    float ndcY = (1.0f - (screen.y - framebufferPos.y) / framebufferSize.y) * 2.0f - 1.0f;

    glm::vec4 clipSpace = glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

    glm::mat4 invProj = glm::inverse(app.camera->GetProjection());
    glm::mat4 invView = glm::inverse(app.camera->GetView());

    glm::vec4 worldPos = invView * invProj * clipSpace;

    return glm::vec2(worldPos.x / worldPos.w, worldPos.y / worldPos.w);
}
