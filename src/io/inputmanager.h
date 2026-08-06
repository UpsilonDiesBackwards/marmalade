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

#ifndef ENGINE_INPUTMANAGER_H
#define ENGINE_INPUTMANAGER_H

#include <GLFW/glfw3.h>

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

#endif
