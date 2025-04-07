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

#include "../window.h"

#include "../msgbox.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <CommCtrl.h>

#include <utility>

void Marmalade::GUI::NativeUI::Window::SetCreateCallback(std::function<void()> createCallback) {
    _create_callback = std::move(createCallback);
}

std::function<void()> Marmalade::GUI::NativeUI::Window::GetCreateCallback() {
    return _create_callback;
}

void Marmalade::GUI::NativeUI::Window::SetClassName(LPCWSTR className) {
    this->_class_name = className;
}

void Marmalade::GUI::NativeUI::Window::SetInstance(HINSTANCE instance) {
    this->_instance = instance;
}

void Marmalade::GUI::NativeUI::Window::SetWndProc(WNDPROC wndProc) {
    this->_wnd_proc = wndProc;
}

void Marmalade::GUI::NativeUI::Window::SetCmdShow(int cmdShow) {
    this->_cmd_show = cmdShow;
}

bool Marmalade::GUI::NativeUI::Window::Create() {
    if (nullptr == this->_class_name) {
        return false;
    }

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;

    // Initialize common controls
    if (!InitCommonControlsEx(&icex)) {
        return false;
    }

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    if (nullptr != _wnd_proc) {
        wcex.lpfnWndProc = this->_wnd_proc;
    }
    if (nullptr != _instance) {
        wcex.hInstance = this->_instance;
    }
    wcex.lpszClassName = this->_class_name;
    wcex.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);

    if (0 == RegisterClassEx(&wcex)) {
        return false;
    }

    this->_handle = CreateWindowEx(
            0, this->_class_name, Marmalade::GUI::NativeUI::Util::utf16ToPlatformStr(this->_title),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, this->_width, this->_height,
            nullptr, nullptr, this->_instance, this);

    return false;
}

bool Marmalade::GUI::NativeUI::Window::Show() {
    ShowWindow(this->_handle, this->_cmd_show);
    return false;
}

LRESULT CALLBACK Marmalade::GUI::NativeUI::Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            if (1 == HIWORD(wParam)) {

            } else {
            }
            return 0;
        case WM_CREATE:
            auto create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
            auto window = reinterpret_cast<Window*>(create_struct->lpCreateParams);

            if (nullptr != window) {
                // Assign handle, since WM_CREATE is fired before CreateWindowEx returns
                if (nullptr == window->GetHandle()) window->_handle = hWnd;

                // Call onCreate callback
                if (nullptr != window->GetCreateCallback()) window->GetCreateCallback()();
            }

            // Set font for all child windows
            auto default_font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
            EnumChildWindows(
                    hWnd, [](HWND hWnd, LPARAM lParam) -> BOOL {
                        auto default_font = reinterpret_cast<HFONT>(lParam);
                        SendMessage(hWnd, WM_SETFONT, WPARAM(default_font), TRUE);
                        return TRUE;
                    },
                    reinterpret_cast<LPARAM>(default_font));

            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Marmalade::GUI::NativeUI::Window Marmalade::GUI::NativeUI::Window::WrapGlfwWindow(GLFWwindow* window) {
    return {glfwGetWin32Window(window)};
}
