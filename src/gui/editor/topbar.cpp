/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "topbar.h"

#include "../../application/application.h"
#include "../../application/util.h"
#include "../../project/projectmanager.h"
#include "../../application/config/configutil.h"
#include "../../application/i18n.h"

#include <ecs/component.h>

#include <imgui.h>

#include <ImGuiFileDialog.h>

#include <IconsCodicons.h>

#include <libintl.h>

void Marmalade::GUI::TopBar::Show() {
    static bool showStyleEditor = false;
    static bool showSceneCreationPopUp = false;
    static char sceneNameBuffer[256] = "";
    static bool showSceneOpenPopUp = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(pgettext("Menu|", "File"))) {
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_ADD, pgettext("Menu|File|", "New Project")))) {
                WindowManager::GetInstance().projectWizard.ToggleWindow();
            }
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_FOLDER_OPENED, pgettext("Menu|File|", "Open Project")))) {
                IGFD::FileDialogConfig config;
                config.path = EngineConfig::GetStoredConfig().defaultProjectPath;
                config.fileName = "project.marmalade";
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project File", ".marmalade", config);
            }
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_SCREEN_FULL, pgettext("Menu|File|", "New Scene")))) {
                showSceneCreationPopUp = true;
            }
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_OPEN_PREVIEW, pgettext("Menu|File|","Open Scene")))) {
                showSceneOpenPopUp = true;
            }
            // TODO: Package Builder menu item, to open existing package builder window
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_STAR, pgettext("Menu|File|", "Welcome Screen")))) {
                WindowManager::GetInstance().welcomeScreen.ToggleWindow();
            }
            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_CLOSE_ALL, pgettext("Menu|File|",  "Quit")))) {
                glfwSetWindowShouldClose(Application::GetInstance().getWindow(), true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(pgettext("Menu|", "Entity"))) {
            auto* inspectedEntity = Application::GetInstance().editorGUI->details.inspectedEntity;
            if (inspectedEntity == nullptr) {
                ImGui::MenuItem(_("No entity selected"), nullptr, nullptr, false);
            } else {
                if (ImGui::BeginMenu(pgettext("Menu|Entity|AddComponent|","Add Component"))) {
                    if (ImGui::BeginMenu(pgettext("Menu|Entity|AddComponent|", "Favourites"))) {
                        for (const auto& component: Marmalade::ECS::ComponentRegistry::Instance().GetFavorites()) {
                            if (ImGui::MenuItem(component->Name.c_str())) {
                                inspectedEntity->componentManager.AddComponent(component->Factory->Create(Util::GenerateUUIDv4()));
                            }
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu(pgettext("Menu|Entity|AddComponent|", "All"))) {
                        for (const auto& [_, component]: Marmalade::ECS::ComponentRegistry::Instance().GetRegisteredComponents()) {
                            if (ImGui::MenuItem(component.Name.c_str())) {
                                inspectedEntity->componentManager.AddComponent(component.Factory->Create(Util::GenerateUUIDv4()));
                            }
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::Separator();
                    for (const auto& [category, components]: Marmalade::ECS::ComponentRegistry::Instance().GetCategoryTree()) {
                        if (ImGui::BeginMenu(category.c_str())) {
                            for (const auto& component: components) {
                                if (ImGui::MenuItem(component->Name.c_str())) {
                                    inspectedEntity->componentManager.AddComponent(component->Factory->Create(Util::GenerateUUIDv4()));
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem(pgettext("Menu|Entity|AddComponent|", "Add Component..."))) {
                        Application::GetInstance().editorGUI->details.SetAddingComponent(true);
                    }

                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(pgettext("Menu|", "Settings"))) {
            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_SETTINGS,  pgettext("Menu|Settings|", "Project Settings")), nullptr, &WindowManager::GetInstance().settings.visible);
            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_EDIT, pgettext("Menu|Settings|", "Style Editor")), nullptr, &showStyleEditor);
            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_SETTINGS_GEAR, pgettext("Menu|Settings|", "Preferences")), nullptr, &WindowManager::GetInstance().preferences.visible);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(pgettext("Menu|", "Window"))) {
            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_DEVICE_CAMERA_VIDEO, pgettext("Menu|Window|", "Animation")), nullptr, &WindowManager::GetInstance().animationManager.visible);

            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_PACKAGE, pgettext("Menu|Window|", "Package Manager")), nullptr, &WindowManager::GetInstance().packageManager.visible);

            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_FILE_TEXT, pgettext("Menu|Window|", "Log")), nullptr, &WindowManager::GetInstance().log.visible);

            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_GIT_COMMIT, pgettext("Menu|Window|", "Version Control")), nullptr, &WindowManager::GetInstance().versionControl.visible);

            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_SAVE, pgettext("Menu|Window|", "Save Layout")))) {
                Application::GetInstance().styleManager.SaveStyle((Marmalade::ConfigUtil::GetConfigDirectory() / Marmalade::EngineConfig::GetStoredConfig().appearance.themeFile).string());

                ImGui::OpenPopup("LayoutSavePopup");
            }

            if (ImGui::BeginPopup("LayoutSavePopup")) {
                ImGui::Text("Saved editor layout");
                ImGui::EndPopup();
            }

            if (ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_DEBUG, pgettext("Menu|Window|", "ImGui Demo")))) {
                WindowManager::GetInstance().ToggleDebugWindow();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(pgettext("Menu|", "Help"))) {
            ImGui::MenuItem(ICON_WITH_TEXT(ICON_CI_INFO, pgettext("Menu|Help|", "About")), nullptr, &WindowManager::GetInstance().about.visible);

            ImGui::EndMenu();
        }

#ifdef DEBUG
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Debug mode");

        if (Util::IsDebuggerAttached()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Debugger attached");
        }
#endif

        if (WindowManager::GetInstance().showDebugWindow) ImGui::ShowDemoWindow();

        float windowWidth = ImGui::GetContentRegionAvail().x;

        ImVec2 editorButtonSize(80, 0);
        float buttonSpacing = 40.0f;

        std::string fpsText = std::format(
                "FPS: {} | ({:.2f} ms)",
                Application::GetInstance().time.GetCurrentFPS(),
                Application::GetInstance().time.GetCurrentFrameTime()
        );

        float fpsRegionWidth = 5.0f;
        float rightMargin = -115.0f;

        float totalButtonWidth = (editorButtonSize.x + editorButtonSize.x) * 3;
        float totalWidth = (totalButtonWidth + fpsRegionWidth + rightMargin);

        ImGui::SetCursorPosX(windowWidth - totalWidth + 550.0f);

        if (Application::GetInstance().playState == PlayState::PlayState_PLAY || Application::GetInstance().playState == PlayState::PlayState_PAUSE) {
            if (ImGui::Button("Stop", editorButtonSize)) {
                Application::GetInstance().playState = PlayState::PlayState_STOP; // Change application play state

                // Load scene again to revert any changes made in Play mode
                auto& sceneManager = Application::GetInstance().sceneManager;
                Scene* scene = Application::GetInstance().sceneManager.GetCurrentScene().get();
                Marmalade::Project::Project* project = Application::GetInstance().GetCurrentProject();

                // Save selected entity UUID
                std::string entityUuid;
                if (Application::GetInstance().editView->selectedEntity != nullptr) {
                    entityUuid = Application::GetInstance().editView->selectedEntity->uuid;
                    Application::GetInstance().editorGUI->sceneHierarchy.DeselectEntity();
                }

                const std::string fileName = Marmalade::Project::ProjectScenes::GetSceneFileName(scene->GetUuid());
                auto newScene = project->scenes.LoadScene(fileName);
                Application::GetInstance().sceneManager.RemoveScene(sceneManager.GetCurrentScene());

                Application::GetInstance().sceneManager.AddScene(std::make_shared<Scene>(newScene));
                Application::GetInstance().sceneManager.SetCurrentScene(newScene.GetUuid());

                // If 'Stop' is pressed, move back over to the edit view
                Application::GetInstance().editorMode = EditorMode::EditorMode_EDIT;

                // Reselect entity
                if (!entityUuid.empty()) {
                    Application::GetInstance().editorGUI->sceneHierarchy.SelectEntityByUuid(entityUuid);
                }
            }
        } else {
            if (ImGui::Button("Play", editorButtonSize)) {
                Application::GetInstance().playState = PlayState::PlayState_PLAY;

                // Serialise Scene
                Scene* scene = Application::GetInstance().sceneManager.GetCurrentScene().get();
                Marmalade::Project::Project* project = Application::GetInstance().GetCurrentProject();

                const std::string fileName = Marmalade::Project::ProjectScenes::GetSceneFileName(scene->GetUuid());
                project->scenes.RegisterScene(fileName);
                project->scenes.SaveScene(fileName, scene);
                project->projectMarmalade->SaveConfig();

                // If 'Play' is pressed, move over to the game view
                Application::GetInstance().editorMode = EditorMode::EditorMode_GAME;
            }
        }

        // Only enable the Pause/Resume button game is in play / paused
        bool isPlaying = (Application::GetInstance().playState == PlayState::PlayState_PLAY || Application::GetInstance().playState == PlayState::PlayState_PAUSE);
        if (!isPlaying) { ImGui::BeginDisabled(); }

        if (Application::GetInstance().playState == PlayState::PlayState_PAUSE) {
            if (ImGui::Button("Resume", editorButtonSize)) {
                Application::GetInstance().playState = PlayState::PlayState_PLAY;
            }
        } else {
            if (ImGui::Button("Pause", editorButtonSize)) {
                Application::GetInstance().playState = PlayState::PlayState_PAUSE;
            }
        }

        if (!isPlaying) { ImGui::EndDisabled(); }

        // Only enable the Step button game is in paused
        bool isPaused = (Application::GetInstance().playState == PlayState::PlayState_PAUSE);
        if (!isPaused) { ImGui::BeginDisabled(); }

        if (ImGui::Button("Step", editorButtonSize)) { // Put the game in to play...
            Application::GetInstance().playState = PlayState::PlayState_STEP;
            LOG_INFO("Stepped one frame");
        }

        if (!isPaused) { ImGui::EndDisabled(); }

        ImGui::SetCursorPosX(windowWidth - fpsRegionWidth - rightMargin - -350.0f);
        ImGui::Text("%s", fpsText.c_str());

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

    for (const auto& window: WindowManager::GetInstance().windows) {
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
                auto uuid = Util::GenerateUUIDv4();
                auto scene = std::make_shared<Scene>(name, uuid);
                Application::GetInstance().sceneManager.AddScene(scene);
                Application::GetInstance().sceneManager.SetCurrentScene(uuid);

                memset(sceneNameBuffer, 0, sizeof(sceneNameBuffer));

                showSceneCreationPopUp = false;
            } else {
                ImGui::Text("Invalid scene name.");
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Open Scene", &showSceneOpenPopUp, ImGuiWindowFlags_AlwaysAutoResize)) {
        Marmalade::Project::Project* project = Application::GetInstance().GetCurrentProject();
        static auto scenes = project->scenes.GetScenes();

        for (const auto& scene: scenes) {
            if (ImGui::Button(scene.GetName().c_str())) {
                auto& sceneManager = Application::GetInstance().sceneManager;

                Application::GetInstance().editorGUI->sceneHierarchy.DeselectEntity();

                const std::string fileName = Marmalade::Project::ProjectScenes::GetSceneFileName(scene.GetUuid());
                auto newScene = project->scenes.LoadScene(fileName);

                sceneManager.AddScene(std::make_shared<Scene>(newScene));
                sceneManager.SetCurrentScene(newScene.GetUuid());

                showSceneOpenPopUp = false;
            }
        }

        if (ImGui::Button("Cancel")) {
            showSceneOpenPopUp = false;
        }

        ImGui::EndPopup();
    }
}
