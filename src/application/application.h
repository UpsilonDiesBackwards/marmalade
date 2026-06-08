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

#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "../project/project.h"

#include "../gui/editor/gameviewport.h"
#include "../gui/windows/stylemanager.h"
#include "../gui/editor/editorviewport.h"
#include "../gui/editor/topbar.h"
#include "../gui/editor/editor.h"
#include "../gui/editor.h"
#include "../gui/nativeui/app.h"
#include "guilogsink.h"

#include "time.h"

#include <GLFW/glfw3.h>

#if DEBUG
#include <imgui_te_engine.h>
#include <imgui_te_context.h>
#endif

#include <scene/entity.h>
#include <graphics/multisampledframebuffer.h>
#include "io/audiomanager.h"
#include "autosave.h"
#include "crashreporter/framecapturer.h"

#include <io/inputmanager.h>
#include <io/input.h>
#include <graphics/viewport.h>

#include <scene/scenemanager.h>

#include <spdlog/spdlog.h>

#include "physics/2d/physicsengine2d.h"

enum PlayState {
    PlayState_PLAY,
    PlayState_STOP,
    PlayState_PAUSE,
    PlayState_STEP
};

enum EditorMode {
    EditorMode_EDIT,
    EditorMode_GAME,
};

class Editor;

/**
 * \brief Main marmalade application class
 */
class Application {
public:
    /**
     * \brief Returns the application singleton instance
     */
    static Application& GetInstance(int width = 1920, int height = 1080, const char* title = "Engine") {
        static Application instance(width, height, title);
        return instance;
    }
    ~Application();

    /**
     * \brief Initialises the project on startup. Performs tasks such as initialising and loading engine config
     */
    void Initialise();

    /**
     * \brief Initialises the GLFW window. This must be run on the main thread.
     */
    void InitialiseWindow();

    void InitialiseImGui();

    /**
     * \brief Executes functions that need to be called per-frame, such as the input and render loop
     */
    void Run();

    /**
     * \brief Setup ImGui Docking
     */
    void SetupDocking() const;

    /**
     * \brief Setup marmalade engine logger
     */
    void SetupLogger();

    /**
     * \brief Terminate and cleanup application process
     */
    void TerminateImGui();

    void TerminateGlfw();

    /**
     * \brief Returns the glfw application window
     */
    GLFWwindow* getWindow();
    /**
     * \brief Returns the application camera
     */
    Camera* getCamera();

    std::shared_ptr<Marmalade::GUI::NativeUI::App> nativeApp = nullptr;

    /**
     * \brief The PlayState of the engine
     * Possible states: PlayState_PLAY, PlayState_STOP, PlayState_PAUSE, PlayState_STEP
     */
    PlayState playState = PlayState::PlayState_STOP;

    /**
     * \brief The mode of the editor, determines user behaviour such as the ability to edit scene entities
     * Possible states: EditorMode_EDIT, EditorMode_GAME
     */
    EditorMode editorMode = EditorMode::EditorMode_EDIT;

    /**
     * \brief The framebuffer of the application. It is used to render the OpenGL context into an ImGui interface
     */
    MultiSampledFramebuffer* framebuffer;

    /**
     * \brief Main editor UI
     */
    Editor* editorGUI;

    /**
     * \brief Edit view of the engine
     */
    EditView* editView;

    /**
     * \brief Game view of the engine
     */
    GameView* gameView;

    AudioManager* audioManager;

    /**
     * \brief The Marmalade input manager. This is used to perform input checks (ie. isKeyDown(), getMouseX())
     */
    InputManager& inputManager;
    Input input;

    /**
     * \brief Marmalade Scene manager
     */
    SceneManager sceneManager;
    Camera* camera;

    StyleManager styleManager;

    Time time;

    AutoSave autoSave;

    std::shared_ptr<GuiLogSink> guiSink;


    bool enableDebugMenu = false;

    const char* COMMIT = COMMIT_HASH;

    Marmalade::FrameCapturer frameCapturer{};

    /**
     * \brief Opens a marmalade project and sets it as the current application project
     */
    bool OpenProject(const std::filesystem::path& path);

    void SetCurrentProject(std::unique_ptr<Marmalade::Project::Project>& project);
    Marmalade::Project::Project* GetCurrentProject();

    /**
     * \brief Performs non-cleanup code such as saving the engine config
     */
    void OnClose();

    /**
     * \brief Used to determine whether the application should advance a single frame during Game mode
     */
    bool stepFrame = false;

    void ChangeWorkspace();

    bool NeedsImGuiRestart();

#if DEBUG
    ImGuiTestEngine* GetImGuiTestEngine() { return _imguiTestEngine; }
#endif

    int GetFramebufferWidth() const;
    int GetFramebufferHeight() const;

private:
    Marmalade::Physics::PhysicsEngine2D physicsEngine2D;

    Application(int width, int height, const char* title);

    int _graphicsVersionMajor;
    int _graphicsVersionMinor;

    GLFWwindow* window;
    int width, height;
    const char* title;

    bool firstRun{false};
    bool firstLoop{true};

    int maxLogSizeMB = 16;
    int maxLogFiles = 3;

    std::shared_ptr<spdlog::logger> logger;

    std::unique_ptr<Marmalade::Project::Project> currentProject;

    bool _requestWorkspaceChange = false;

#if DEBUG
    ImGuiTestEngine* _imguiTestEngine = nullptr;

    void registerImGuiTests();
#endif

    /**
     * \brief Gets the graphics version from the engine config. Ambiguous name, should be changed
     */
    void getGraphicsVersion();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

#define GET_APP Application::GetInstance()

#endif
