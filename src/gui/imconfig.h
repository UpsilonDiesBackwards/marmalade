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

#ifndef MARMALADE_GUI_IMCONFIG_H
#define MARMALADE_GUI_IMCONFIG_H

#define IMGUI_DEFINE_MATH_OPERATORS

#if DEBUG

#define IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL 1
#define IMGUI_TEST_ENGINE_ENABLE_IMPLOT 1

#include <imgui_te_imconfig.h>

#define IMGUI_ENABLE_TEST_ENGINE

#endif

#endif
