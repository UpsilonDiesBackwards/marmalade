#include <iostream>
#include "../../include/io/input.h"
#include "../../include/io/inputmanager.h"

Input::Input(InputManager *inputManager) : inputManager(inputManager) {
}

void Input::BindKey(int key, KeyEventType eventType, std::function<void()> action) {
    keyBinds[{key, eventType}] = action;
}

void Input::BindMouseButton(int button, MouseEventType eventType, std::function<void()> action) {
    mouseButtonBinds[{button, eventType}] = action;
}

void Input::Update() {
    inputManager->Update();

    for (const auto& [keyEventPair, action] : keyBinds) {
        int key = keyEventPair.first;
        KeyEventType eventType = keyEventPair.second;

        if ((eventType == KEY_DOWN    && inputManager->isKeyDown(key))  ||
            (eventType == KEY_UP      && inputManager->isKeyUp(key))    ||
            (eventType == KEY_PRESSED && inputManager->isKeyPressed(key))) {
            action();
        }
    }

    for (const auto& [mouseEventPair, action] : mouseButtonBinds) {
        int button = mouseEventPair.first;
        MouseEventType eventType = mouseEventPair.second;

        if ((eventType == MOUSE_DOWN    && inputManager->isMouseButtonDown(button))    ||
            (eventType == MOUSE_UP      && inputManager->isMouseButtonUp(button))      ||
            (eventType == MOUSE_PRESSED && inputManager->isMouseButtonPressed(button))) {
            action();
        }
    }

    if (scrollAction) {
        double scrollX = inputManager->getScrollDeltaX();
        double scrollY = inputManager->getScrollDeltaY();

        if (scrollX != 0.0 || scrollY != 0.0) {
            scrollAction(scrollX, scrollY);
        }
    }
    inputManager->resetScroll();
}

void Input::BindScroll(std::function<void(double, double)> action) {
    scrollAction = action;
}
