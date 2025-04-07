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

#ifndef MARMALADE_GUI_NATIVEUI_UTIL_H
#define MARMALADE_GUI_NATIVEUI_UTIL_H

#include <string>

#ifdef _WIN32

#include <windows.h>

#define PLATFORM_STRING_TYPE LPCWSTR

#elif __APPLE__

#include <Cocoa/Cocoa.h>
#define PLATFORM_STRING_TYPE NSString*

#else

#define PLATFORM_STRING_TYPE char*

#endif

namespace Marmalade::GUI::NativeUI {
    class Util {
    public:
        static PLATFORM_STRING_TYPE utf16ToPlatformStr(const std::u16string& u16String);

        static std::u16string utf8ToUtf16Str(const std::basic_string<char>& str);
    };
}

#endif
