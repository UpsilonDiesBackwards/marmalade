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

#include "inputmanager.h"

InputManager::InputManager() {
    for (int i = 0; i < 512; i++) {
        keys[i] = false;
        prevKeys[i] = false;
    }

    for (int i = 0; i < 8; ++i) {
        mouseButtons[i] = false;
        prevMouseButtons[i] = false;
    }

    mouseX = 0.0f;
    mouseY = 0.0f;
}

InputManager::~InputManager() {
}

InputManager &InputManager::GetInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::SetWindow(GLFWwindow *targetWindow) {
    window = targetWindow;

    initKeyCallbacks();
    initMouseCallbacks();
}

bool InputManager::isKeyPressed(int key) {
    return keys[key];
}

bool InputManager::isKeyDown(int key) {
    return keys[key] && prevKeys[key];
}

bool InputManager::isKeyUp(int key) {
    return !keys[key] && prevKeys[key];
}

bool InputManager::isMouseButtonPressed(int button) {
    return mouseButtons[button];
}

bool InputManager::isMouseButtonDown(int button) {
    return mouseButtons[button] && !prevMouseButtons[button];
}

bool InputManager::isMouseButtonUp(int button) {
    return !mouseButtons[button] && prevMouseButtons[button];
}

double InputManager::getMouseX() {
    return mouseX;
}

double InputManager::getMouseY() {
    return mouseY;
}

double InputManager::getMouseDeltaX() const {
    return mouseDeltaX;
}

double InputManager::getMouseDeltaY() const {
    return mouseDeltaY;
}

double InputManager::getScrollDeltaX() const {
    return scrollDeltaX;
}

double InputManager::getScrollDeltaY() const {
    return scrollDeltaY;
}

void InputManager::resetScroll() {
    scrollDeltaX = 0.0f;
    scrollDeltaY = 0.0f;
}

void InputManager::Update() {
    for (int i = 0; i < 512; i++) {
        prevKeys[i] = keys[i];
    }

    for (int i = 0; i < 8; ++i) {
        prevMouseButtons[i] = mouseButtons[i];
    }

    mouseDeltaX = mouseX - prevMouseX;
    mouseDeltaY = mouseY - prevMouseY;

    prevMouseX = mouseX;
    prevMouseY = mouseY;
}

void InputManager::initKeyCallbacks() {
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        InputManager& inputManager = InputManager::GetInstance();
        if (action == GLFW_PRESS) {
            inputManager.keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            inputManager.keys[key] = false;
        }
    });
}

void InputManager::initMouseCallbacks() {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        InputManager& inputManager = InputManager::GetInstance();
        if (action == GLFW_PRESS) {
            inputManager.mouseButtons[button] = true;
        } else if (action == GLFW_RELEASE) {
            inputManager.mouseButtons[button] = false;
        }
    });

    // Set up mouse position callback
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        InputManager& inputManager = InputManager::GetInstance();
        inputManager.mouseX = xpos;
        inputManager.mouseY = ypos;
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
        InputManager& inputManager = InputManager::GetInstance();
        inputManager.scrollDeltaX += xOffset;
        inputManager.scrollDeltaY += yOffset;
    });
}
