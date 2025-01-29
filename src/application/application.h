#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "GLFW/glfw3.h"
#include "../../include/archetypes/entity.h"
#include "../../include/graphics/multisampledframebuffer.h"
#include "../gui/gameview.h"
#include "../gui/stylemanager.h"
#include "../gui/editview.h"
#include "../gui/imgui/topbar.h"
#include "../../include/io/inputmanager.h"
#include "../../include//io/input.h"
#include "profiler.h"
#include "../../include/archetypes/camera.h"
#include "../gui/editorviews.h"
#include "../../include/scene/scenemanager.h"
#include "../gui/imgui/editor.h"

enum PlayState {
    Play, Stop,
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
private:
    Application(int width, int height, const char* title);

    GLFWwindow* window;
    int width, height;
    const char* title;
};
#endif //ENGINE_APPLICATION_H
