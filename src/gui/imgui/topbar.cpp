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
                projectWizard.ToggleWindow();
            }
            if (ImGui::MenuItem(ICON_CI_FOLDER_OPENED " Open Project")) {
                IGFD::FileDialogConfig config;
                config.path = Config::engineConfig.defaultProjectPath;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project Directory", nullptr, config);
            }
            if (ImGui::MenuItem(ICON_CI_SCREEN_FULL " New Scene")) {
                showSceneCreationPopUp = true;
            }
            if (ImGui::MenuItem(ICON_CI_OPEN_PREVIEW " Open Scene")) {
                showSceneOpenPopUp = true;
            }
            if (ImGui::MenuItem(ICON_CI_CLOSE_ALL " Quit")) {
                glfwSetWindowShouldClose(Application::GetInstance().getWindow(), true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::Button("Save")) {
        }

        if (ImGui::BeginMenu("Settings")) {
            ImGui::MenuItem(ICON_CI_SETTINGS " Project Settings", nullptr, &settings.visible);
            ImGui::MenuItem(ICON_CI_EDIT " Style Editor", nullptr, &showStyleEditor);
            ImGui::MenuItem(ICON_CI_SETTINGS_GEAR " Preferences", nullptr, &preferences.visible);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem(ICON_CI_PACKAGE " Package Manager", nullptr, &packageManager.visible);

            ImGui::MenuItem(ICON_CI_FILE_TEXT " Log", nullptr, &log.visible);

            if (ImGui::MenuItem(ICON_CI_SAVE " Save Layout")) {
                Application::GetInstance().styleManager.SaveStyle((Marmalade::Config::GetConfigDirectory() / Marmalade::Config::engineConfig.ThemeFile).string());

                ImGui::OpenPopup("LayoutSavePopup");
            }

            if (ImGui::BeginPopup("LayoutSavePopup")) {
                ImGui::Text("Saved editor layout");
                ImGui::EndPopup();
            }

            if (ImGui::MenuItem(ICON_CI_DEBUG " ImGui Demo")) {
                showDebugWindow = !showDebugWindow;
            }

            ImGui::EndMenu();
        }

        if (showDebugWindow) ImGui::ShowDemoWindow();

        float alignRight = ImGui::GetWindowWidth() - 170;
        ImGui::SameLine(alignRight);

        ImGui::Text("FPS: %d | (%.2f ms)", Application::GetInstance().profiler.GetCurrentFPS(),
                    Application::GetInstance().profiler.GetCurrentFrameTime());

        if (ImGuiFileDialog::Instance()->Display("ChooseProject")) {
            static std::string selectedProject = "";

            if (ImGuiFileDialog::Instance()->IsOk()) {
                selectedProject  = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::filesystem::path projectPath = selectedProject;

                if (std::filesystem::is_directory(projectPath)) {
                    std::filesystem::path settingsFile = projectPath / "settings.marm"; // this should be changed to project.marmalade
                    spdlog::info("settings path: {}", settingsFile.string());

                    if (std::filesystem::exists(settingsFile)) {
                        GitSettings gitSettings;

                        auto project = std::make_unique<Project>(selectedProject, projectPath.string(), gitSettings, true);

                        Application::GetInstance().SetCurrentProject(project);

                        Settings::LoadProjectSettings();
                    } else {
                        spdlog::error("Project Settings file not found");
                    }
                } else {
                    spdlog::error("Provided path is not a valid project directory");
                }
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

    for (const auto& window: windows) {
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
