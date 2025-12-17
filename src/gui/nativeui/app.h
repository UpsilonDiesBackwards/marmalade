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

#ifndef MARMALADE_GUI_NATIVEUI_APP_H
#define MARMALADE_GUI_NATIVEUI_APP_H

#include "nativeobject.h"

#include <functional>

#if _WIN32

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

using app_handle_type_t = void*;

#elif __APPLE__

#include "darwin/DarwinObjWrapper.h"

using app_handle_type_t = Marmalade::GUI::NativeUI::DarwinObjWrapper<void*>*;

#else

#include <gtk-3.0/gtk/gtk.h>

using app_handle_type_t = GtkApplication*;

#endif

namespace Marmalade::GUI::NativeUI {
    class App : public NativeObject<app_handle_type_t> {
    public:
        App() = default;

        void Create(int argc, char** argv);

        void RunOnMainThread(std::function<void()> func);

        void Terminate();

        void SetCreateCallback(std::function<void(app_handle_type_t)> createCallback) {
            _createCallback = createCallback;
        }

        std::function<void(app_handle_type_t)> GetCreateCallback() {
            return _createCallback;
        }

    private:
        std::function<void(app_handle_type_t)> _createCallback = nullptr;
    };
}

#endif
