
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

        Matrix() {
            for (size_t i = 0; i < Rows * Columns; ++i) {
                values[i] = T(0);
            }
        }

        template<typename... Args, typename = std::enable_if_t<(sizeof...(Args) == Columns * Columns)>>
        Matrix(Args... args) {
            T temp[] = { static_cast<T>(args)... };
            for (size_t i = 0; i < Columns * Columns; ++i) {
                values[i] = temp[i];
            }
        }

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
            for (std::size_t row = 0; row < Rows; ++row) {
                for (std::size_t col = 0; col < Columns; ++col) {
                    T sum = static_cast<T>(0);
                    for (std::size_t k = 0; k < Columns; ++k) {
                        sum += (*this)(row, k) * m(k, col);
                    }
                    r(row, col) = sum;
                }
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

        T& operator()(std::size_t row, std::size_t col) {
            return values[col * Rows + row];
        }

        const T& operator()(std::size_t row, std::size_t col) const {
            return values[col * Rows + row];
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

        float* GetValues() {
            return values;
        }

        const float* GetConstValues() const {
            return values;
        }

        static Matrix Identity() {
            Matrix r;
            for (size_t col = 0; col < Columns; ++col) {
                for (size_t row = 0; row < Rows; ++row) {
                    r(col, row) = (row == col) ? 1 : 0;
                }
            }
            return r;
        }

        static Matrix<4, 4, T> Translate(const Vector<3, T>& translate) {
            Matrix<4, 4, T> r = Identity();
            r.values[12] = translate[0];
            r.values[13] = translate[1];
            r.values[14] = translate[2];
            return r;
        }

        static Matrix<4, 4, T> Scale(const Vector<3, T>& scale) {
            Matrix<4, 4, T> r = Identity();
            r.values[0] = scale[0];
            r.values[5] = scale[1];
            r.values[10] = scale[2];

            return r;
        }

        static Matrix<4, 4, T> Rotate(T angle, const Vector<3, T>& axis) {
            T c = std::cos(angle);
            T s = std::sin(angle);

            Vector<3, T> normAxis = axis.Normalise();
            T x = normAxis[0], y = normAxis[1], z = normAxis[2];
            T oneMinusC = 1 - c;

            Matrix<4, 4, T> rot = Identity();
            rot.values[0] = c + x * x * oneMinusC;
            rot.values[1] = x * y * oneMinusC - z * s;
            rot.values[2] = x * z * oneMinusC + y * s;

            rot.values[4] = y * x * oneMinusC + z * s;
            rot.values[5] = c + y * y * oneMinusC;
            rot.values[6] = y * z * oneMinusC - x * s;

            rot.values[8] = z * x * oneMinusC - y * s;
            rot.values[9] = z * y * oneMinusC + x * s;
            rot.values[10] = c + z * z * oneMinusC;

            return rot;
        }
    };

    using Mat2 = Matrix<2,2>;
    using Mat3 = Matrix<3,3>;
    using Mat4 = Matrix<4,4>;
}

#endif//MARMALADE_MATRIX_H
