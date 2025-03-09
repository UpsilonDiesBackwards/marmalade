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

#include <glad/glad.h>

#include "application.h"

#include "config.h"
#include "recents.h"
#include "util.h"
#include "../project/projectmanager.h"

#include <imgui.h>
#include <imgui_internal.h>// Only for docking API
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>
#include <fstream>

Application::Application(int width, int height, const char* title) : window(nullptr), width(width), height(height), title(title), inputManager(InputManager::GetInstance()),
                                                                     input(&inputManager), camera(new Camera(1920, 1080, 1.0f)) {

    sceneManager = SceneManager();
    profiler = Profiler();
    editorGUI = new Editor;
}

Application::~Application() {
    //    Terminate();
}

void Application::Initialise() {
    SetupLogger();

    static std::filesystem::path imguiIniPath = Marmalade::Config::GetConfigDirectory() / "imgui.ini";
    static std::string imguiIniPathStr = imguiIniPath.string();

    std::ifstream imguiIni(imguiIniPath);
    firstRun = !imguiIni.good();
    imguiIni.close();

    if (!glfwInit()) {// Initialise GLFW
        std::cerr << "Failed to Initialise GLFW!" << std::endl;
        return;
    } else
        std::cout << "GLFW Initialised" << std::endl;

    window = glfwCreateWindow(width, height, title, NULL, NULL);// Create the main application window
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    } else
        std::cout << "Window Created" << std::endl;
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    inputManager.SetWindow(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {// Initialise GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } else
        std::cout << "GLAD Initialised" << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();// Create ImGui Context
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Load ImGui custom style

    if (!std::filesystem::exists(Marmalade::Config::GetConfigDirectory() / "editorstyle.txt")) {
        std::filesystem::copy_file("res/config/editorstyle.txt", Marmalade::Config::GetConfigDirectory() / "editorstyle.txt");
    }

    styleManager.LoadStyle((Marmalade::Config::GetConfigDirectory() / Marmalade::Config::engineConfig.themeFile).string());

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = imguiIniPathStr.c_str();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_None | ImGuiConfigFlags_DockingEnable;
    if (Marmalade::Config::engineConfig.viewports) {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }
    ImFont* font = io.Fonts->AddFontFromFileTTF("res/fonts/Cabin/static/Cabin-Regular.ttf", 16);

    static const ImWchar icons_ranges[] = {ICON_MIN_CI, ICON_MAX_16_CI, 0};

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    float font_size = 16.0f;
    icons_config.GlyphMinAdvanceX = font_size;
    icons_config.GlyphOffset.y = 3.0f;
    io.Fonts->AddFontFromFileTTF("res/fonts/codicon.ttf", font_size, &icons_config, icons_ranges);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    gameView = new GameView(width, height);
    editView = new EditView(width, height);

    const auto uuid = Marmalade::Util::GenerateUUIDv4();
    auto scene = std::make_shared<Scene>("Default", uuid);
    sceneManager.AddScene(scene);
    sceneManager.SetCurrentScene(uuid);

    Marmalade::ECS::Transform defaultTransform;
    Entity* newEntity = new Entity("New Entity", EntityFlags::RENDERABLE);
    sceneManager.GetCurrentScene()->AddEntity(std::shared_ptr<Entity>(newEntity));
}

void Application::Run() {
    profiler.Update();

    ImVec4 backgroundCol = ImGui::ColorConvertU32ToFloat4(Marmalade::Config::engineConfig.backgroundColor);
    glClearColor(backgroundCol.x, backgroundCol.y, backgroundCol.z, backgroundCol.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    SetupDocking();

    input.Update();

    editorGUI->Render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwPollEvents();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backupContext = glfwGetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        glfwMakeContextCurrent(backupContext);
    }

    glfwSwapBuffers(window);

    firstLoop = false;
}

void Application::SetupDocking() const {
    if (firstRun && firstLoop) {
        ImVec2 logPos = ImVec2(180, 720);
        ImVec2 logSize = ImVec2(1600, 320);
        ImVec2 logCenter{logPos.x + logSize.x * 0.5f, logPos.y + logSize.y * 0.5f};

        ImGuiID dockerId = ImGui::GetID("LogDock");
        ImGui::DockBuilderRemoveNode(dockerId);
        ImGui::DockBuilderAddNode(dockerId);

        ImVec2 nodePos{logCenter.x - logSize.x * 0.5f, logCenter.y - logSize.y * 0.5f};

        ImGui::DockBuilderSetNodeSize(dockerId, logSize);
        ImGui::DockBuilderSetNodePos(dockerId, nodePos);

        ImGui::DockBuilderDockWindow(ICON_CI_FILE_TEXT " Engine Log", dockerId);
        ImGui::DockBuilderDockWindow(ICON_CI_FILE_TEXT " Game Log", dockerId);

        ImGui::DockBuilderFinish(dockerId);
    }
}

void Application::Terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Application::getWindow() {
    return window;
}

Camera* Application::getCamera() {
    return camera;
}

void Application::SetupLogger() {
    auto maxSize = 1048576 * maxLogSizeMB;
    auto maxFiles = maxLogFiles;

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto engineLogPath = Marmalade::Config::GetConfigDirectory() / "engine-log.txt";
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            engineLogPath.string(), maxSize, maxFiles);

    guiSink = std::make_shared<GuiLogSink>();

    logger = std::make_shared<spdlog::logger>("EngineLogger",
                                              spdlog::sinks_init_list{consoleSink, fileSink, guiSink});
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::set_level(Marmalade::Config::engineConfig.logLevel);
}

bool Application::OpenProject(const std::filesystem::path& path) {
    try {
        auto project = std::make_unique<Marmalade::Project::Project>(Marmalade::Project::ProjectManager<>::OpenProject(path));
        Marmalade::Recents::AddRecentProject(Marmalade::RecentProject{project->projectMarmalade.name, project->projectMarmalade.uuid, path.string()});
        Marmalade::Recents::SaveRecents();

        SetCurrentProject(project);
    } catch (const std::exception& ex) {
        spdlog::error("Failed to open project: {}", ex.what());
        return false;
    }

    return true;
}

void Application::SetCurrentProject(std::unique_ptr<Marmalade::Project::Project>& project) {// Change the current projects and update the window title to inc project name
    currentProject = std::move(project);

    std::string windowTitle = std::string(title) + " // " + currentProject->name;

    glfwSetWindowTitle(window, windowTitle.c_str());
}

Marmalade::Project::Project* Application::GetCurrentProject() {
    return currentProject.get();
}
