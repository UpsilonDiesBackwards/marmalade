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

#include "util.h"

#include <cuchar>
#include <stdexcept>

std::u16string Marmalade::GUI::NativeUI::Util::utf8ToUtf16Str(const std::basic_string<char>& str) {
    std::mbstate_t state = std::mbstate_t();
    std::u16string utf16str;
    char16_t c16;
    const char* src = str.c_str();
    size_t len;

    while ((len = mbrtoc16(&c16, src, MB_CUR_MAX, &state)) > 0) {
        if (len == -1 || len == -2) {
            throw std::runtime_error("UTF-8 to UTF-16 conversion error");
        }

        utf16str += c16;
        src += len;
    }

    return utf16str;
}
