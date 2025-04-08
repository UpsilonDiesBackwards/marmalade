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

#ifndef MARMALADE_GUI_NATIVEUI_WINDOW_H
#define MARMALADE_GUI_NATIVEUI_WINDOW_H

#include <GLFW/glfw3.h>

#include <iostream>
#include <utility>
#include <algorithm>
#include <functional>

#include "nativeobject.h"
#include "util.h"

#if _WIN32

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

using win_handle_type_t = HWND;

#elif __APPLE__

#include "darwin/DarwinObjWrapper.h"

using win_handle_type_t = Vineyard::UI::DarwinObjWrapper<NSWindow*>*;

#else

#include <gtk-3.0/gtk/gtk.h>

using win_handle_type_t = GtkWidget*;

#endif

namespace Marmalade::GUI::NativeUI {
    class Window : public NativeObject<win_handle_type_t> {
    public:
        Window(std::u16string title, int width, int height) : _title(std::move(title)), _width(width),
                                                              _height(height) {
#if _WIN32
            std::replace_copy(_title.begin(), _title.end(), std::back_inserter(_class_name_str), ' ', '_');
            _class_name = Util::utf16ToPlatformStr(_class_name_str);
#endif
        };

        Window(win_handle_type_t handle) {
            _handle = handle;
        }

        static Window WrapGlfwWindow(GLFWwindow* window);

        void SetCreateCallback(std::function<void()> createCallback);

        std::function<void()> GetCreateCallback();

        const std::u16string GetTitle() const {
            return _title;
        }

        const int GetWidth() const {
            return _width;
        }

        const int GetHeight() const {
            return _height;
        }

#if _WIN32

        void SetClassName(LPCWSTR className);

        void SetInstance(HINSTANCE instance);

        void SetWndProc(WNDPROC wndProc);

        void SetCmdShow(int cmdShow);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#elif __APPLE__

        ~Window();

#else
        void SetApp(GtkApplication* app);
#endif

        bool Create();

        bool Show(bool topmost = false);

        void Close();

    private:
        std::u16string _title;

        int _width;

        int _height;

        std::function<void()> _create_callback = nullptr;

#if defined(WIN32)
        LPCWSTR _class_name = nullptr;

        std::u16string _class_name_str = std::u16string();

        HINSTANCE _instance = nullptr;

        WNDPROC _wnd_proc = WndProc;

        int _cmd_show = SW_SHOWNORMAL;
#elif defined(__linux__)
        GtkApplication* _app = nullptr;
#endif
    };
}

#endif
