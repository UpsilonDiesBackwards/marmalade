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

#include <mathematics/types/vector3.h>

#include <cmath>
#include <format>
#include <string>

using namespace Marmalade::Mathematics;

Vector3 Vector3::operator+(const Vector3& vec) const {
    return Vector3(vec.x+x, vec.y+y, vec.z+z);
}
Vector3 Vector3::operator-(const Vector3& vec) const {
    return Vector3(vec.x-x, vec.y-y, vec.z-z);
}
Vector3 Vector3::operator*(const Vector3& vec) const {
    return Vector3(vec.x*x, vec.y*y, vec.z*z);
}
Vector3 Vector3::operator/(const Vector3& vec) const {
    return Vector3(vec.x/x, vec.y/y, vec.z/z);
}
float Vector3::Magnitude() {
    return sqrt(x*x + y*y + z*z);
}

std::string Vector3::ToString() {
    return std::format("{}, {}, {}", x, y, z);
}
