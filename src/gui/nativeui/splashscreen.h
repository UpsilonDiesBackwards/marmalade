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

#ifndef MARMALADE_GUI_NATIVEUI_SPLASHSCREEN_H
#define MARMALADE_GUI_NATIVEUI_SPLASHSCREEN_H

#include "window.h"

namespace Marmalade::GUI::NativeUI {
    class SplashScreen {
    public:
        static void Create(Window& window);

        static void Paint(Window& window);

        static void SetLoadingText(Window &window, std::string text) {
            _loadingText = text;
            updateLoadingText(window);
        }

        static std::string GetLoadingText() {
            return _loadingText;
        }

    private:
        static std::string _loadingText;

        static void updateLoadingText(Window &window);
    };
}


#endif
