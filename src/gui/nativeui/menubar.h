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

#ifndef MARMALADE_GUI_NATIVEUI_MENUBAR_H
#define MARMALADE_GUI_NATIVEUI_MENUBAR_H

#include <functional>
#include <map>
#include <stack>

namespace Marmalade::GUI::NativeUI {
    class MenuBar {
    public:
        static bool BeginMainMenuBar();
        static void EndMainMenuBar();
        static bool BeginMenu(const char *label);
        static void EndMenu();
        static bool MenuItem(const char *label);
        static bool MenuItem(const char *label, const char* shortcut, bool* p_selected, bool enabled = true);
        static void Separator();

    private:
        static std::stack<void*> _menuStack;
        static std::map<std::string, bool> _clickStates;
        static std::map<std::string, void*> _nativeItems;
        static std::vector<void*> _menuTargets;
    };
}

#endif
