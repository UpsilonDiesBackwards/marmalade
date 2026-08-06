// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "inputmanager.h"

#include <functional>
#include <map>

class InputManager;

enum KeyEventType {
    KEY_DOWN,
    KEY_UP,
    KEY_PRESSED
};

enum MouseEventType {
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_PRESSED
};

class Input {
public:
    Input(InputManager *inputManager);

    void BindKey(int key, KeyEventType eventType, std::function<void()> action);
    void BindMouseButton(int button, MouseEventType eventType, std::function<void()> action);
    void BindScroll(std::function<void(double, double)> action);

    void UnbindKey(int key, KeyEventType eventType);
    void UnbindMouseButton(int button, MouseEventType eventType);
    void UnbindScroll();

    void ClearAllInputEvents();

    void Update();
private:
    InputManager *inputManager;
    std::map<std::pair<int, KeyEventType>, std::function<void()>> keyBinds;
    std::map<std::pair<int, MouseEventType>, std::function<void()>> mouseButtonBinds;
    std::function<void(double, double)> scrollAction;
};

#endif
