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

#include "integration.h"
#include "logger.h"
#include "config/engineconfig.h"
#include "config/configutil.h"
#include "config/recents.h"
#include "util.h"
#include "../project/projectmanager.h"
#include "../project/assetregistry.h"
#include "../gui/fontmanager.h"

#include <imgui.h>
#include <imgui_internal.h>// Only for docking API
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <imnodes.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <stb/stb_image.h>

#include <iostream>
#include <fstream>

/**
 * \param width Width of the application window
 * \param height Height of the application window
 * \param title Title of the application window - Most often times set the name of the current project
 *
 * \brief Application constructor, sets necessary variables
 */
Application::Application(int width, int height, const char* title) : window(nullptr), width(width), height(height), title(title),
    inputManager(InputManager::GetInstance()), input(&inputManager), camera(new Camera(1920, 1080, 1.0f)), audioManager(&AudioManager::GetInstance()) {

    sceneManager = SceneManager();
    time = Time();
    editorGUI = new Editor;
}

Application::~Application() {
    //    Terminate();
}

void Application::Initialise() {
    SetupLogger();

    static std::filesystem::path imguiIniPath = Marmalade::ConfigUtil::GetConfigDirectory() / "imgui.ini";
    static std::string imguiIniPathStr = imguiIniPath.string();

    std::ifstream imguiIni(imguiIniPath);
    firstRun = !imguiIni.good();
    imguiIni.close();

    if (!glfwInit()) {// Initialise GLFW
        std::cerr << "Failed to Initialise GLFW!" << std::endl;
        return;
    } else
        std::cout << "GLFW Initialised" << std::endl;

    getGraphicsVersion();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _graphicsVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _graphicsVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);// macOS requires this

    window = glfwCreateWindow(width, height, title, NULL, NULL);// Create the main application window
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    } else
        std::cout << "Window Created" << std::endl;
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWimage images[1];
    images[0].pixels = stbi_load("res/icons/logo/logo192.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    int monitorNum = Marmalade::EngineConfig::GetStoredConfig().windowPos.monitor;
    if (monitorNum > -1) {
        // Full screen
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        glfwSetWindowMonitor(window, monitors[monitorNum], 0, 0, width, height, GLFW_DONT_CARE);
    } else {
        int x = Marmalade::EngineConfig::GetStoredConfig().windowPos.x;
        int y = Marmalade::EngineConfig::GetStoredConfig().windowPos.y;

        int configWidth = Marmalade::EngineConfig::GetStoredConfig().windowPos.width;
        int configHeight = Marmalade::EngineConfig::GetStoredConfig().windowPos.height;

        glfwSetWindowPos(window, x, y);
        glfwSetWindowSize(window, configWidth, configHeight);

        if (Marmalade::EngineConfig::GetStoredConfig().windowPos.maximised) glfwMaximizeWindow(window);
    }

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

    std::stringstream versionStream;
    versionStream << "#version " << _graphicsVersionMajor << _graphicsVersionMinor << "0";
    auto versionStr = versionStream.str();
    ImGui_ImplOpenGL3_Init(versionStr.c_str());

    ImNodes::CreateContext();
    ImNodes::StyleColorsDark();

    // Load ImGui custom style
    if (!std::filesystem::exists(Marmalade::ConfigUtil::GetConfigDirectory() / "editorstyle.txt")) {
        std::filesystem::copy_file("res/config/editorstyle.txt", Marmalade::ConfigUtil::GetConfigDirectory() / "editorstyle.txt");
    }

    styleManager.LoadStyle((Marmalade::ConfigUtil::GetConfigDirectory() / Marmalade::EngineConfig::GetStoredConfig().appearance.themeFile).string());

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = imguiIniPathStr.c_str();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_None | ImGuiConfigFlags_DockingEnable;
    if (Marmalade::EngineConfig::GetStoredConfig().appearance.viewports) {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    Marmalade::GUI::FontManager::GetInstance().InitFonts();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    gameView = new GameView(width, height);
    editView = new EditView(width, height);

    const auto uuid = Marmalade::Util::GenerateUUIDv4();
    auto scene = std::make_shared<Scene>("Default", uuid);
    sceneManager.AddScene(scene);
    sceneManager.SetCurrentScene(uuid);

    Marmalade::ECS::Transform defaultTransform;
    Entity* newEntity = new Entity("New Entity");
    sceneManager.GetCurrentScene()->AddEntity(std::shared_ptr<Entity>(newEntity));

    // Build component category tree
    Marmalade::ECS::ComponentRegistry::Instance().BuildCategoryTree();
    for (const auto& component: Marmalade::EngineConfig::GetStoredConfig().favouriteComponents) {
        Marmalade::ECS::ComponentRegistry::Instance().SetFavourite(component);
    }
}

void Application::Run() {
    time.Update();
    time.FixedUpdate();

    ImVec4 backgroundCol = ImGui::ColorConvertU32ToFloat4(Marmalade::EngineConfig::GetStoredConfig().appearance.backgroundColor);
    glClearColor(backgroundCol.x, backgroundCol.y, backgroundCol.z, backgroundCol.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    SetupDocking();

    input.Update();

    // Turn empty background into a dock space
    ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
    ImGui::DockSpaceOverViewport(dockspaceId, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    editorGUI->Render();

    if (playState == PlayState::PlayState_STEP) {
        playState = PlayState::PlayState_PLAY;
        stepFrame = true;
    }

    if (stepFrame) { // Step one frame
        playState = PlayState::PlayState_PAUSE;
        stepFrame = false;
    }

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

    ImNodes::DestroyContext();

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
    auto engineLogPath = Marmalade::ConfigUtil::GetConfigDirectory() / "engine-log.txt";
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            engineLogPath.string(), maxSize, maxFiles);

    guiSink = std::make_shared<GuiLogSink>();

    logger = std::make_shared<spdlog::logger>("EngineLogger",
                                              spdlog::sinks_init_list{consoleSink, fileSink, guiSink});
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::set_level(Marmalade::EngineConfig::GetStoredConfig().logLevel);
}

/**
 * \brief
 * \param path Path of the target project to open
 * \return Boolean
 */

bool Application::OpenProject(const std::filesystem::path& path) {
    try {
        auto project = std::make_unique<Marmalade::Project::Project>(Marmalade::Project::ProjectManager<>::OpenProject(path));
        Marmalade::Application::Integration::MarkRecentFile(path);
        Marmalade::Recents::GetInstance().AddRecentProject(Marmalade::RecentProject{project->projectMarmalade->storedConfig.name, project->projectMarmalade->storedConfig.uuid, path.string()});
        Marmalade::Recents::GetInstance().SaveConfig();

        SetCurrentProject(project);

        std::filesystem::path assetRegistry = currentProject->basePath / ".assetreg";

        if (!std::filesystem::exists(assetRegistry)) {
            LOG_WARN("Asset Registry missing, creating new one");

            try {
                std::ofstream createdFile(assetRegistry);
                if (!createdFile) {
                    LOG_ERROR("Error creating new asset registry: '{}'", assetRegistry.string());
                    return false;
                }

                Marmalade::Project::Assets::Registry::GetInstance().RebuildRegistry();
                Marmalade::Project::Assets::Registry::GetInstance().Save(assetRegistry);
                createdFile.close();
            } catch (const std::exception& e) {
                LOG_ERROR("Error creating file '{}': {}", assetRegistry.string(), e.what());
                return false;
            }
        } else {
            Marmalade::Project::Assets::Registry::GetInstance().Load(assetRegistry);
        }
    } catch (const std::exception& ex) {
        LOG_ERROR("Failed to open project: {}", ex.what());
        return false;
    }

    return true;
}

/**
 * \brief Change from the current project to a new project
 * \param project Target project
 */

void Application::SetCurrentProject(std::unique_ptr<Marmalade::Project::Project>& project) {// Change the current projects and update the window title to inc project name
    currentProject = std::move(project);

    std::string windowTitle = std::string(title) + " - " + currentProject->projectMarmalade->storedConfig.name;

    glfwSetWindowTitle(window, windowTitle.c_str());
}

/**
 * \brief Returns the currently opened project
 * \return Marmalade::Project::Project* current application
 */
Marmalade::Project::Project* Application::GetCurrentProject() {
    return currentProject.get();
}

void Application::OnClose() {
    glfwGetWindowPos(window,
                     &Marmalade::EngineConfig::GetStoredConfig().windowPos.x,
                     &Marmalade::EngineConfig::GetStoredConfig().windowPos.y);
    glfwGetWindowSize(window,
                      &Marmalade::EngineConfig::GetStoredConfig().windowPos.width,
                      &Marmalade::EngineConfig::GetStoredConfig().windowPos.height);

    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    GLFWmonitor* monitor = glfwGetWindowMonitor(window);
    if (monitor == nullptr) {
        Marmalade::EngineConfig::GetStoredConfig().windowPos.monitor = -1;
    } else {
        for (int i = 0; i < monitorCount; i++) {
            if (monitors[i] == monitor) {
                Marmalade::EngineConfig::GetStoredConfig().windowPos.monitor = i;
            }
        }
    }

    Marmalade::EngineConfig::GetInstance().SaveConfig();
}

/**
 * \brief Get the Graphics version from the engine config
 */

void Application::getGraphicsVersion() {
#undef interface // WIN32 COM causing problems
    std::stringstream ss(Marmalade::EngineConfig::GetStoredConfig().interface.graphicsVersion);
    int major, minor;

    char dot;
    ss >> major >> dot >> minor;

    _graphicsVersionMajor = major;
    _graphicsVersionMinor = minor;
}
