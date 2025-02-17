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

#include "../gui/gameview.h"
#include "../gui/stylemanager.h"
#include "../gui/editview.h"
#include "../gui/imgui/topbar.h"
#include "../gui/editorviews.h"
#include "../gui/imgui/editor.h"
#include "guilogsink.h"
#include "profiler.h"

#include <scene/entity.h>
#include <graphics/multisampledframebuffer.h>
#include <io/inputmanager.h>
#include <io/input.h>
#include <graphics/viewport.h>
#include <scene/scenemanager.h>

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

enum PlayState {
    Play,
    Stop,
};

enum EditorMode {
    EDIT,
    GAME,
};

class Editor;

class Application {
public:
    static Application& GetInstance(int width = 1920, int height = 1080, const char* title = "Engine") {
        static Application instance(width, height, title);
        return instance;
    }
    ~Application();

    void Initialise();
    void Run();
    void SetupDocking() const;
    void SetupLogger();
    void Terminate();

    GLFWwindow* getWindow();
    Camera* getCamera();

    PlayState playState = PlayState::Stop;
    EditorMode editorMode = EditorMode::EDIT;

    MultiSampledFramebuffer* framebuffer;

    Editor* editorGUI;

    EditView* editView;
    GameView* gameView;

    InputManager& inputManager;
    Input input;

    SceneManager sceneManager;
    Camera* camera;

    StyleManager styleManager;

    Profiler profiler;

    std::shared_ptr<GuiLogSink> guiSink;

    void SetCurrentProject(std::unique_ptr<Marmalade::Project::Project>& project);
    Marmalade::Project::Project* GetCurrentProject();

private:
    Application(int width, int height, const char* title);

    GLFWwindow* window;
    int width, height;
    const char* title;

    bool firstRun{false};
    bool firstLoop{true};

    int maxLogSizeMB = 16;
    int maxLogFiles = 3;

    std::shared_ptr<spdlog::logger> logger;

    std::unique_ptr<Marmalade::Project::Project> currentProject;
};

#endif
