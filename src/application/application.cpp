#include <iostream>
#include "glad/glad.h"
#include "application.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

Application::Application(int width, int height, const char *title) :
    window(nullptr), width(width), height(height), title(title), inputManager(InputManager::GetInstance()),
    input(&inputManager), camera(new Camera(1920, 1080, 1.0f)) {

    sceneManager = SceneManager();
    profiler = Profiler();
    editorGUI = new Editor;
}

Application::~Application() {
//    Terminate();
}

void Application::Initialise() {
    if (!glfwInit()) { // Initialise GLFW
        std::cerr << "Failed to Initialise GLFW!" << std::endl;
        return;
    } else std::cout << "GLFW Initialised" << std::endl;

    window = glfwCreateWindow(width, height, title, NULL, NULL); // Create the main application window
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    } else std::cout << "Window Created" << std::endl;

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
    styleManager.LoadStyle("../res/config/editorstyle.txt");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "../res/config/imgui.ini";
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_None | ImGuiConfigFlags_DockingEnable;
    ImFont* font = io.Fonts->AddFontFromFileTTF("../res/fonts/monofur/monof55.ttf", 16);

//    glEnable(GL_DEPTH_TEST);

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

    // TODO: EDITOR STUFF
//    ImGui::ShowStyleEditor();

    input.Update();

    editorGUI->Render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwPollEvents();
    glfwSwapBuffers(window);
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
