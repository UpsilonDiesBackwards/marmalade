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

#include <mathematics/types/vector2.h>

#include <format>
#include <cmath>

using namespace Marmalade::Mathematics;

Vector2 Vector2::operator+(const Vector2& vec) const {
    return Vector2(vec.x+x, vec.y+y);
}
Vector2 Vector2::operator-(const Vector2& vec) const {
    return Vector2(vec.x-x, vec.y-y);
}
Vector2 Vector2::operator*(const Vector2& vec) const {
    return Vector2(vec.x*x, vec.y*y);
}
Vector2 Vector2::operator/(const Vector2& vec) const {
    return Vector2(vec.x/x, vec.y/y);
}

float Vector2::Magnitude() {
    return sqrt(x*x + y*y); // Ought to use our own sqrt func at some point
}

std::string Vector2::ToString() {
    return std::format("{}, {}", x, y);
}
