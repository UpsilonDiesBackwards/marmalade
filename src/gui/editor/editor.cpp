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

#include "imgui.h"

#include <iostream>

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