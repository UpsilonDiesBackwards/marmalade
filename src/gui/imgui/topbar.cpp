#include "topbar.h"
#include "../../application/application.h"

#include <imgui.h>

#include <IconsCodicons.h>

void Marmalade::GUI::TopBar::Show() {
    static bool showStyleEditor = false;
    static bool showSceneCreationPopUp = false;
    static char sceneNameBuffer[32] = "";
    static bool showSceneOpenPopUp = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_CI_ADD " New Project")) {
            }
            if (ImGui::MenuItem(ICON_CI_FOLDER_OPENED " Open Project")) {
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
            if (ImGui::MenuItem(ICON_CI_EDIT " Style Editor")) {
                showStyleEditor = !showStyleEditor;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem(ICON_CI_PACKAGE " Package Manager")) {
                packageManager.ToggleWindow();
            }

            if (ImGui::MenuItem(ICON_CI_FILE_TEXT " Log")) {
                log.ToggleWindow();
            }

            if (ImGui::MenuItem(ICON_CI_SAVE " Save Layout")) {
                Application::GetInstance().styleManager.SaveStyle();

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
