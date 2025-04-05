
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

#ifndef MARMALADE_ABS_H
#define MARMALADE_ABS_H

#include "mathematics/vector.h"

namespace Marmalade::Mathematics {
    template<std::size_t C, typename T>
    class Vector;

    template<typename T>
    constexpr T Abs(T value) {
        return (value < 0) ? -value : value;
    }

    template<std::size_t C, typename T>
    Vector<C, T> Abs(const Vector<C, T>& v) {
        Vector<C, T> r;
        for (std::size_t i = 0; i < C; ++i) {
            r[i] = Abs(v[i]);
        }
        return r;
    }
}

#endif//MARMALADE_ABS_H
