//
// Created by tayler on 24/12/24.
//

#ifndef ENGINE_INPUTMANAGER_H
#define ENGINE_INPUTMANAGER_H


#include "GLFW/glfw3.h"

class InputManager {
public:
    static InputManager& GetInstance();

    bool isKeyPressed(int key);
    bool isKeyDown(int key);
    bool isKeyUp(int key);

    bool isMouseButtonPressed(int button);
    bool isMouseButtonDown(int button);
    bool isMouseButtonUp(int button);

    double getMouseX();
    double getMouseY();

    double getMouseDeltaX() const;
    double getMouseDeltaY() const;

    double getScrollDeltaX() const;
    double getScrollDeltaY() const;
    void resetScroll();

    void Update();

    void SetWindow(GLFWwindow* targetWindow);
private:
    InputManager();
    ~InputManager();

    bool keys[512];
    bool mouseButtons[8];

    double mouseX, mouseY;
    double mouseDeltaX, mouseDeltaY;
    double prevMouseX, prevMouseY;

    double scrollDeltaX, scrollDeltaY;

    bool prevKeys[512];
    bool prevMouseButtons[8];

    void initKeyCallbacks();
    void initMouseCallbacks();

    GLFWwindow* window;
};


#endif //ENGINE_INPUTMANAGER_H
