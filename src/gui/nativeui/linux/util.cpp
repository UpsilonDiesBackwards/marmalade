// Copyright 2024 Ryan Bester, Tayler Parsons
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../util.h"

#include <gtk-3.0/gtk/gtk.h>

PLATFORM_STRING_TYPE Marmalade::GUI::NativeUI::Util::utf16_to_platform_str(const std::u16string& utf16_str) {
    GError* g_error = nullptr;
    auto utf8_str = g_utf16_to_utf8(
            reinterpret_cast<const gunichar2*>(utf16_str.c_str()), -1, nullptr,
            nullptr, &g_error);

    if (nullptr != g_error) {
        g_error_free(g_error);
        if (nullptr != error) *error = ERR_UTF16_STR_CONV_FAIL;
        return nullptr;
    }

    return utf8_str;
}
