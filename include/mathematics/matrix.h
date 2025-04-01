
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

namespace Marmalade::Mathematics {
    template<std::size_t Rows, std::size_t Columns, typename T = float>
    class Matrix {
    public:
        T values[Rows * Columns]{};

        constexpr std::size_t getRows() const { return Rows; }
        constexpr std::size_t getColumns() const { return Columns; }

        template<std::size_t R, std::size_t C>
        Matrix operator+(const Matrix<R, C, T>& m) {
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
        Matrix operator-(const Matrix<R, C, T>& m) {
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
        Matrix operator*(const Matrix<R, C, T>& m) {
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
        Matrix operator/(const Matrix<R, C, T>& m) {
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
    };

    using Mat2 = Matrix<2,2>;
    using Mat3 = Matrix<4,4>;
    using Mat4 = Matrix<4,4>;
}

#endif//MARMALADE_MATRIX_H
