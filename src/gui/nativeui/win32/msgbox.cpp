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

#include "../msgbox.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

using namespace Marmalade::GUI::NativeUI;

using Style = MsgBox::Style;
using Buttons = MsgBox::Buttons;
using Result = MsgBox::Result;

Result win32CreateDialog(HWND wnd, const std::u16string& message,  const std::u16string& title,  Style style, Buttons buttons) {
    UINT s = 0;
    UINT btn = MB_OK;

    switch (style) {
        case Style::Style_INFO:
            s = MB_ICONASTERISK;
            break;
        case Style::Style_WARNING:
            s = MB_ICONWARNING;
            break;
        case Style::Style_ERROR:
            s = MB_ICONERROR;
            break;
        case Style::Style_QUESTION:
            s = MB_ICONQUESTION;
            break;
    }

    switch (buttons) {
        case Buttons::Buttons_OK:
            btn = MB_OK;
            break;
        case Buttons::Buttons_OK_CANCEL:
            btn = MB_OKCANCEL;
            break;
        case Buttons::Buttons_YES_NO:
            btn = MB_YESNO;
            break;
    }

    int res = MessageBox(
            wnd, Util::utf16ToPlatformStr(message),
            Util::utf16ToPlatformStr(title), s | btn);
    switch (res) {
        case IDOK:
            return Result::Result_OK;
        case IDCANCEL:
            return Result::Result_CANCEL;
        case IDYES:
            return Result::Result_YES;
        case IDNO:
            return Result::Result_NO;
        default:
            return Result::Result_NONE;
    }
}

Result MsgBox::ShowMessage(const std::u16string& message,  const std::u16string& title,   Style style, Buttons buttons) {
    return win32CreateDialog(nullptr, message, title, style, buttons);
}

Result MsgBox::ShowMessage(const Window& window, const std::u16string& message,  const std::u16string& title,   Style style,Buttons buttons) {
    return win32CreateDialog(window.GetHandle(), message, title, style, buttons);
}
