#include <glad/glad.h>

#include "application.h"

#include "config.h"

#include <imgui.h>
#include <imgui_internal.h>// Only for docking API
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <IconsCodicons.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>
#include <fstream>

#include <IconsCodicons.h>

Application::Application(int width, int height, const char *title) :
    window(nullptr), width(width), height(height), title(title), inputManager(InputManager::GetInstance()),
    input(&inputManager), camera(new Camera(1920, 1080, 1.0f)), currentProject("Default Project", "", false) {

    sceneManager = SceneManager();
    profiler = Profiler();
    editorGUI = new Editor;
}

Application::~Application() {
//    Terminate();
}

void Application::Initialise() {
    SetupLogger();

    std::ifstream imguiIni("imgui.ini");
    firstRun = !imguiIni.good();
    imguiIni.close();

    if (!glfwInit()) {// Initialise GLFW
        std::cerr << "Failed to Initialise GLFW!" << std::endl;
        return;
    } else std::cout << "GLFW Initialised" << std::endl;

    window = glfwCreateWindow(width, height, title, NULL, NULL); // Create the main application window
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    } else std::cout << "Window Created" << std::endl;
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    inputManager.SetWindow(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Initialise GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } else std::cout << "GLAD Initialised" << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); // Create ImGui Context
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Load ImGui custom style
    styleManager.LoadStyle("res/config/editorstyle.txt");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_None | ImGuiConfigFlags_DockingEnable;
    if (Marmalade::Config::engineConfig.Viewports) {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }
    ImFont* font = io.Fonts->AddFontFromFileTTF("res/fonts/monofur/monof55.ttf", 16);

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

    auto scene = std::make_shared<Scene>("Default");
    sceneManager.AddScene(scene);
    sceneManager.SetCurrentScene("Default");

    Transform defaultTransform;
    Entity *newEntity = new Entity("test entity", EntityFlags::RENDERABLE, defaultTransform);
    sceneManager. GetCurrentScene()->AddEntity(std::shared_ptr<Entity>(newEntity));
}

void Application::Run() {
    profiler.Update();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    SetupDocking();

    if (editorMode == EditorMode::EDIT) {
        editView->RunInput();
    }

    if (editorMode == EditorMode::GAME) {
        gameView->RunInput();
    }

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

GLFWwindow *Application::getWindow() {
    return window;
}

Camera *Application::getCamera() {
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
    spdlog::set_level(spdlog::level::debug);
}

void Application::SetCurrentProject(Project project) { // Change the current projects and update the window title to inc project name
    currentProject = project;

    std::string windowTitle = std::string(title) + " // " + currentProject.name;

    glfwSetWindowTitle(window, windowTitle.c_str());
}

Project Application::GetCurrentProject() {
    return currentProject;
}