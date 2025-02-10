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

#ifndef MARMALADE_VECTOR4_H
#define MARMALADE_VECTOR4_H

#include <string>

namespace Marmalade::Mathematics {
    struct Vector4 {
    public:
        float x, y, z, w;
        
        Vector4(float x, float y, float z, float(w)) : x(x), y(y), z(z), w(w) { };

        Vector4 operator+(Vector4 const& vec) const;
        Vector4 operator-(Vector4 const& vec) const;
        Vector4 operator*(Vector4 const& vec) const;
        Vector4 operator/(Vector4 const& vec) const;

        float Magnitude();

        std::string ToString();
    };
}


#endif
