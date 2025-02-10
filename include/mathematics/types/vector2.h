/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_VECTOR2_H
#define MARMALADE_VECTOR2_H

#include <string>

namespace Marmalade::Mathematics {
    struct Vector2 {
    public:
        float x, y;

        Vector2(float x, float y) : x(x), y(y) { };

        Vector2 operator+(Vector2 const& vec) const;
        Vector2 operator-(Vector2 const& vec) const;
        Vector2 operator*(Vector2 const& vec) const;
        Vector2 operator/(Vector2 const& vec) const;

        float Magnitude();

        std::string ToString();
    };
}

#endif
