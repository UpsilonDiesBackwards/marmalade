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

#ifndef MARMALADE_PLUGINS_GUI_H
#define MARMALADE_PLUGINS_GUI_H

#include <engineapi.h>
#include <interface.h>

#include <imgui.h>

#include <string>
#include <unordered_map>
#include <functional>

namespace Marmalade {
    class InterfaceApiImpl {
    public:
        static inline InterfaceAPI Create() {
            return InterfaceAPI{
                    .Init = &Init,
                    .AddBeginHook = &AddBeginHook,
                    .AddEndHook = &AddEndHook,
                    .GetFocusedWindow = &GetFocusedWindow,
#define INTERFACE_API_INITIALISER
#include <imgui.inc>
#undef INTERFACE_API_INITIALISER
            };
        }

        static void Init() {}

        static void AddBeginHook(const char* nameRegex, void (*BeginHook)());

        static void AddEndHook(const char* nameRegex, void (*EndHook)());

        static const char* GetFocusedWindow();

#define INTERFACE_API_IMPL
#include <imgui.inc>
#undef INTERFACE_API_IMPL

        static void CallBeginHooks(std::string windowName);

        static void CallEndHooks(std::string windowName);

        static void SetFocusedWindow(std::string windowName);

    private:
        static std::string _focusedWindowName;

        static std::unordered_map<std::string, bool> _hookCacheStatus;

        static std::unordered_map<std::string, std::vector<std::function<void()>>> _beginHooksCached;
        static std::unordered_map<std::string, std::vector<std::function<void()>>> _endHooksCached;

        static std::unordered_map<std::string, std::vector<std::function<void()>>> _beginHooks;
        static std::unordered_map<std::string, std::vector<std::function<void()>>> _endHooks;

        static void cacheHooks(std::string windowName);
        static bool inCache(std::string windowName);
    };
}

#endif
