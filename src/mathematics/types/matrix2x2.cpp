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

#include <mathematics/types/matrix2x2.h>

#include <format>

using namespace Marmalade::Mathematics;

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& mA) const {
    return {
            m[0][0] + mA.m[0][0], m[0][1] + mA.m[0][1],
            m[1][0] + mA.m[1][0], m[1][1] + mA.m[1][1]
    };
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& mA) const {
    return {
            m[0][0] - mA.m[0][0], m[0][1] - mA.m[0][1],
            m[1][0] - mA.m[1][0], m[1][1] - mA.m[1][1]
    };
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& mA) const {
    return {
            m[0][0] * mA.m[0][0] + m[0][1] * mA.m[1][0],
            m[0][0] * mA.m[0][1] + m[0][1] * mA.m[1][1],

            m[1][0] * mA.m[0][0] + m[1][1] * mA.m[1][0],
            m[1][0] * mA.m[0][1] + m[1][1] * mA.m[1][1],
    };
}

std::string Matrix2x2::ToString() {
    return std::format("[{}, {}] [{}, {}]", m[0][0], m[0][1], m[1][0], m[1][1]);
}
