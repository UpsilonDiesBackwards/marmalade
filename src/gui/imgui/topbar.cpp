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

#include "topbar.h"

#include "../../application/application.h"
#include "../../application/recents.h"
#include "../../project/projectmanager.h"
#include "../windowmanager.h"

#include <imgui.h>

#include <ImGuiFileDialog.h>

#include <IconsCodicons.h>

void Marmalade::GUI::TopBar::Show() {
    static bool showStyleEditor = false;
    static bool showSceneCreationPopUp = false;
    static char sceneNameBuffer[32] = "";
    static bool showSceneOpenPopUp = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_CI_ADD " New Project")) {
                WindowManager::GetInstance().projectWizard.ToggleWindow();
            }
            if (ImGui::MenuItem(ICON_CI_FOLDER_OPENED " Open Project")) {
                IGFD::FileDialogConfig config;
                config.path = Config::engineConfig.defaultProjectPath;
                config.fileName = "project.marmalade";
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project File", ".marmalade", config);
            }
            if (ImGui::MenuItem(ICON_CI_SCREEN_FULL " New Scene")) {
                showSceneCreationPopUp = true;
            }
            if (ImGui::MenuItem(ICON_CI_OPEN_PREVIEW " Open Scene")) {
                showSceneOpenPopUp = true;
            }
            if (ImGui::MenuItem(ICON_CI_STAR " Welcome Screen")) {
                WindowManager::GetInstance().welcomeScreen.ToggleWindow();
            }
            if (ImGui::MenuItem(ICON_CI_CLOSE_ALL " Quit")) {
                glfwSetWindowShouldClose(Application::GetInstance().getWindow(), true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::Button("Save")) {
        }

        if (ImGui::BeginMenu("Settings")) {
            ImGui::MenuItem(ICON_CI_SETTINGS " Project Settings", nullptr, &WindowManager::GetInstance().settings.visible);
            ImGui::MenuItem(ICON_CI_EDIT " Style Editor", nullptr, &showStyleEditor);
            ImGui::MenuItem(ICON_CI_SETTINGS_GEAR " Preferences", nullptr, &WindowManager::GetInstance().preferences.visible);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem(ICON_CI_PACKAGE " Package Manager", nullptr, &WindowManager::GetInstance().packageManager.visible);

            ImGui::MenuItem(ICON_CI_FILE_TEXT " Log", nullptr, &WindowManager::GetInstance().log.visible);

            if (ImGui::MenuItem(ICON_CI_SAVE " Save Layout")) {
                Application::GetInstance().styleManager.SaveStyle((Marmalade::Config::GetConfigDirectory() / Marmalade::Config::engineConfig.themeFile).string());

                ImGui::OpenPopup("LayoutSavePopup");
            }

            if (ImGui::BeginPopup("LayoutSavePopup")) {
                ImGui::Text("Saved editor layout");
                ImGui::EndPopup();
            }

            if (ImGui::MenuItem(ICON_CI_DEBUG " ImGui Demo")) {
                WindowManager::GetInstance().ToggleDebugWindow();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem(ICON_CI_INFO " About", nullptr, &WindowManager::GetInstance().about.visible);

            ImGui::EndMenu();
        }

        if (WindowManager::GetInstance().showDebugWindow) ImGui::ShowDemoWindow();

        float alignRight = ImGui::GetWindowWidth() - 170;
        ImGui::SameLine(alignRight);

        ImGui::Text("FPS: %d | (%.2f ms)", Application::GetInstance().profiler.GetCurrentFPS(),
                    Application::GetInstance().profiler.GetCurrentFrameTime());

        if (ImGuiFileDialog::Instance()->Display("ChooseProject")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                auto dir = ImGuiFileDialog::Instance()->GetCurrentPath();
                auto fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                std::filesystem::path projectPath = std::filesystem::path(dir) / fileName;

                Application::GetInstance().OpenProject(projectPath);
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::EndMainMenuBar();
    }

    if (showStyleEditor) {
        ImGui::SetNextWindowPos(ImVec2(86, 53), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(388, 976), ImGuiCond_FirstUseEver);
        ImGui::Begin("Style Editor", &showStyleEditor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    if (showSceneCreationPopUp) {
        ImGui::OpenPopup("New Scene");
    }

    if (showSceneOpenPopUp) {
        ImGui::OpenPopup("Open Scene");
    }

    for (const auto &window : WindowManager::GetInstance().windows){
        window->Show();
    }

    if (ImGui::BeginPopupModal("New Scene", &showSceneCreationPopUp, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Scene Name: ");
        ImGui::InputText("##SceneName", sceneNameBuffer, sizeof(sceneNameBuffer));

        if (ImGui::Button("Cancel")) {
            memset(sceneNameBuffer, 0, sizeof(sceneNameBuffer));
            showSceneCreationPopUp = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Create")) {
            std::string name = std::string(sceneNameBuffer);

            if (!name.empty()) {
                auto scene = std::make_shared<Scene>(name);
                Application::GetInstance().sceneManager.AddScene(scene);
                Application::GetInstance().sceneManager.SetCurrentScene(name);

                memset(sceneNameBuffer, 0, sizeof(sceneNameBuffer));

                showSceneCreationPopUp = false;
            } else {
                ImGui::Text("Invalid scene name.");
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Open Scene", &showSceneOpenPopUp, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Scene Name: ");
        ImGui::InputText("##SceneName", sceneNameBuffer, sizeof(sceneNameBuffer));

        if (ImGui::Button("Cancel")) {
            memset(sceneNameBuffer, 0, sizeof(sceneNameBuffer));
            showSceneOpenPopUp = false;
        }
        ImGui::SameLine();

        if (ImGui::Button("Open")) {
            std::string name = std::string(sceneNameBuffer);

            if (!name.empty()) {
                Application::GetInstance().sceneManager.SetCurrentScene(name);

                memset(sceneNameBuffer, 0, sizeof(sceneNameBuffer));
                showSceneOpenPopUp = false;
            } else {
                ImGui::Text("Empty or invalid scene name.");
            }
        }

        ImGui::EndPopup();
    }
}
