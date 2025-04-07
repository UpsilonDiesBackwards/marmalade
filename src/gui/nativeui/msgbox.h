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

#ifndef MARMALADE_GUI_NATIVEUI_MSGBOX_H
#define MARMALADE_GUI_NATIVEUI_MSGBOX_H

#include <iostream>

#include "window.h"

namespace Marmalade::GUI::NativeUI {
    class MsgBox {
    public:
        enum class Style {
            Style_INFO,
            Style_WARNING,
            Style_ERROR,
            Style_QUESTION
        };

        enum class Buttons {
            Buttons_OK,
            Buttons_OK_CANCEL,
            Buttons_YES_NO
        };

        enum class Result {
            Result_OK,
            Result_CANCEL,
            Result_YES,
            Result_NO,
            Result_NONE
        };

        static Result
        ShowMessage(const Window& window,
                    const std::u16string& message,
                    const std::u16string& title,
                    Style style,
                    Buttons buttons = Buttons::Buttons_OK);

        static Result
        ShowMessage(const std::u16string& message,
                    const std::u16string& title,
                    Style style,
                    Buttons buttons = Buttons::Buttons_OK);
    };
}


#endif
