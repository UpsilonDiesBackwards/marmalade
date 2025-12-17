/*
Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../menubar.h"

#include <imgui.h>

using namespace Marmalade::GUI::NativeUI;

std::stack<void*> MenuBar::_menuStack{};
std::map<std::string, bool> MenuBar::_clickStates{};
std::map<std::string, void*> MenuBar::_nativeItems{};
std::vector<void*> MenuBar::_menuTargets{};

bool MenuBar::BeginMainMenuBar() {
    return ImGui::BeginMainMenuBar();
}

void MenuBar::EndMainMenuBar() {
    ImGui::EndMainMenuBar();
}

bool MenuBar::BeginMenu(const char* label) {
    return ImGui::BeginMenu(label);
}

void MenuBar::EndMenu() {
    ImGui::EndMenu();
}

bool MenuBar::MenuItem(const char* label) {
    return ImGui::MenuItem(label);
}

bool MenuBar::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled) {
    return ImGui::MenuItem(label, shortcut, p_selected, enabled);
}

void MenuBar::Separator() {
    ImGui::Separator();
}
