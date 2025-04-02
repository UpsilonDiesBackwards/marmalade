
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

#ifndef MARMALADE_MATRIX_H
#define MARMALADE_MATRIX_H

#include <concepts>
#include <format>
#include "vector.h"

namespace Marmalade::Mathematics {
    template<std::size_t Rows, std::size_t Columns, typename T = float>
    class Matrix {
    public:
        T values[Rows * Columns]{};

        constexpr std::size_t getRows() const { return Rows; }
        constexpr std::size_t getColumns() const { return Columns; }

        template<std::size_t R, std::size_t C>
        Matrix operator+(const Matrix<R, C, T>& m) const {
            if (Rows != m.getRows() || Columns != m.getColumns()) {
                static_assert(Rows == R && Columns == C, "Cannot perform arithmetic on matrices with different dimensions");
            }

            Matrix r;
            for (std::size_t i = 0; i < Rows * Columns; ++i) {
                r.values[i] = values[i] + m.values[i];
            }

            return r;
        }

        template<std::size_t R, std::size_t C>
        Matrix operator-(const Matrix<R, C, T>& m) const {
            if (Rows != m.getRows() || Columns != m.getColumns()) {
                static_assert(Rows == R && Columns == C, "Cannot perform arithmetic on matrices with different dimensions");
            }

            Matrix r;
            for (std::size_t i = 0; i < Rows * Columns; ++i) {
                r.values[i] = values[i] - m.values[i];
            }

            return r;
        }

        template<std::size_t R, std::size_t C>
        Matrix operator*(const Matrix<R, C, T>& m) const {
            if (Rows != m.getRows() || Columns != m.getColumns()) {
                static_assert(Rows == R && Columns == C, "Cannot perform arithmetic on matrices with different dimensions");
            }

            Matrix r;
            for (std::size_t i = 0; i < Rows * Columns; ++i) {
                r.values[i] = values[i] * m.values[i];
            }

            return r;
        }

        template<std::size_t R, std::size_t C>
        Matrix operator/(const Matrix<R, C, T>& m) const {
                if (Rows != m.getRows() || Columns != m.getColumns()) {
                static_assert(Rows == R && Columns == C, "Cannot perform arithmetic on matrices with different dimensions");
            }

            Matrix r;
            for (std::size_t i = 0; i < Rows * Columns; ++i) {
                r.values[i] = values[i] / m.values[i];
            }

            return r;
        }

        std::string ToString() {
            std::string result = "[";

            for (std::size_t i = 0; i < Rows * Columns; ++i) {
                result += std::format("{}", values[i]);
                if ((i + 1) % Columns == 0) {
                    result += (i == Rows * Columns - 1) ? "]" : "\n";
                } else {
                    result += ", ";
                }
            }

            return result;
        }

        Matrix Identity() {
            Matrix r;

            for (size_t i = 0; i < Columns * Rows; ++i) {
                r.values[i] = 1;
            }

            return r;
        }

        template<std::size_t R>
        Matrix<4, 4, T> Translate(const Marmalade::Mathematics::Vector<3, T>& translate) {
            Matrix<4, 4, T> r;

            r.values[3] = translate.x;
            r.values[7] = translate.y;
            r.values[11] = translate.z;

            return r;
        }

        template<std::size_t R>
        Matrix<4, 4, T> Scale(const Marmalade::Mathematics::Vector<3, T>& scale) {
            Matrix<4, 4, T> result;

            result.values[0] = scale.x;
            result.values[5] = scale.y;
            result.values[10] = scale.z;
            result.values[15] = 1;

            return result;
        }

        Matrix<4, 4, T> Rotate(const Matrix<4, 4, T>& m, T angle, const Marmalade::Mathematics::Vector<3, T>& axis) {
            T c = std::cos(angle);
            T s = std::sin(angle);
            Marmalade::Mathematics::Vector<3, T> normAxis = axis.Normalise();

            T x = normAxis[0];
            T y = normAxis[1];
            T z = normAxis[2];
            T oneMinusC = 1 - c;

            Matrix<4, 4, T> rot;

            rot.values[0] = c + x * x * oneMinusC;
            rot.values[1] = x * y * oneMinusC - z * s;
            rot.values[2] = x * z * oneMinusC + y * s;
            rot.values[3] = 0;

            rot.values[4] = y * x * oneMinusC + z * s;
            rot.values[5] = c + y * y * oneMinusC;
            rot.values[6] = y * z * oneMinusC - x * s;
            rot.values[7] = 0;

            rot.values[8] = z * x * oneMinusC - y * s;
            rot.values[9] = z * y * oneMinusC + x * s;
            rot.values[10] = c + z * z * oneMinusC;
            rot.values[11] = 0;

            rot.values[12] = 0;
            rot.values[13] = 0;
            rot.values[14] = 0;
            rot.values[15] = 1;

            return m * rot;
        }
    };

    using Mat2 = Matrix<2,2>;
    using Mat3 = Matrix<4,4>;
    using Mat4 = Matrix<4,4>;
}

#endif//MARMALADE_MATRIX_H
