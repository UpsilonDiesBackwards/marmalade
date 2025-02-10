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

#include <io/input.h>

#include <io/inputmanager.h>

#include <iostream>

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

void Input::UnbindKey(int key, KeyEventType eventType) {
    keyBinds.erase({key, eventType});
}

void Input::UnbindMouseButton(int button, MouseEventType eventType) {
    mouseButtonBinds.erase({button, eventType});
}

void Input::UnbindScroll() {
    scrollAction = nullptr;
}
