
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

#ifndef MARMALADE_VECTOR_H
#define MARMALADE_VECTOR_H

#include "mathematics/functions/sqrt.h"
#include "matrix.h"

#include <concepts>
#include <format>

namespace Marmalade::Mathematics {
    enum class Unit {
        Radians,
        Degrees
    };

    template<std::size_t Columns, typename T = float>
    class Vector {
    public:
        T values[Columns]{};

        constexpr std::size_t getColumns() const { return Columns; }

        Vector() {
            for (size_t i = 0; i < Columns; ++i) {
                values[i] = T(0);
            }
        }

        template<typename... Args, typename = std::enable_if_t<(sizeof...(Args) == Columns)>>
        Vector(Args... args) {
            T temp[] = { static_cast<T>(args)... };
            for (size_t i = 0; i < Columns; ++i) {
                values[i] = temp[i];
            }
        }

        template<std::size_t C>
        Vector operator+(const Vector<C, T>& v) const {
            if (Columns != v.getColumns()) {
                static_assert(Columns == C, "Can not perform arithmetic on vectors of different sizes");
            }

            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] + v.values[i];
            }

            return r;
        }

        Vector operator+(const float f) const {
            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] + f;
            }

            return r;
        }

        template<std::size_t C>
        Vector operator-(const Vector<C, T>& v) const {
            if (Columns != v.getColumns()) {
                static_assert(Columns == C, "Can not perform arithmetic on vectors of different sizes");
            }

            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] - v.values[i];
            }

            return r;
        }

        template<std::size_t C>
        Vector operator*(const Vector<C, T>& v) const {
            if (Columns != v.getColumns()) {
                static_assert(Columns == C, "Can not perform arithmetic on vectors of different sizes");
            }

            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] * v.values[i];
            }

            return r;
        }

        Vector operator*(float f) const {
            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] * f;
            }
            return r;
        }

        template<std::size_t C>
        Vector operator/(const Vector<C, T>& v) const {
            if (Columns != v.getColumns()) {
                static_assert(Columns == C, "Can not perform arithmetic on vectors of different sizes");
            }

            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] / v.values[i];
            }

            return r;
        }

        Vector operator-() const {
            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = -values[i];
            }
            return r;
        }

        template<std::size_t C>
        Vector& operator+=(const Vector<C, T>& v) {
            static_assert(C == Columns, "Cannot += vectors of different dimensions");
            for (std::size_t i = 0; i < Columns; ++i) {
                values[i] += v[i];
            }
            return *this;
        }

        Vector& operator*=(T scalar) {
            for (std::size_t i = 0; i < Columns; ++i) {
                values[i] *= scalar;
            }
            return *this;
        }

        Vector operator/(T scalar) const {
            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] / scalar;
            }
            return r;
        }

        T& operator[](std::size_t index) {
            return values[index];
        }

        const T& operator[](std::size_t index) const {
            return values[index];
        }

        std::string ToString() {
            std::string result = "[";

            for (std::size_t i = 0; i < Columns; ++i) {
                result += std::format("{}", values[i]);
                if ((i + 1) % Columns == 0) {
                    result += (i == Columns - 1) ? "]" : "\n";
                } else {
                    result += ", ";
                }
            }

            return result;
        }

        float* ToPtr() {
            return values;
        }

        glm::vec4 ToGlmVec4(const Marmalade::Mathematics::Vector<4>& v) {
            return glm::vec4(v[0], v[1], v[2], v[3]);
        }

        constexpr T Sqrt(T value);
        Vector sqrt() {
            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                if (values[i] < 0) {
                    static_assert("Cannot get a square root of a negative number");
                }
                r.values[i] = Marmalade::Mathematics::Sqrt(values[i]);
            }
            return r;
        }

        // Sometimes referred to as the 'length' of a vector
        float Magnitude() const {
            float sum = 0.0f;

            for (std::size_t i = 0; i < Columns; ++i) {
                sum += values[i] * values[i];
            }

            return Marmalade::Mathematics::Sqrt(sum);
        }

        template<std::size_t C>
        float Distance(const Vector<C, T>& a, const Vector<C, T>& b) {
            float sum = 0.0f;

            for (std::size_t i = 0; i < C; ++i) {
                float diff = static_cast<float>(a[i] - b[i]);
                sum += diff * diff;
            }

            return Marmalade::Mathematics::Sqrt(sum);
        }

        Vector Normalise() const {
            float magnitude = Magnitude();

            Vector r;
            for (std::size_t i = 0; i < Columns; ++i) {
                r.values[i] = values[i] / magnitude;
            }

            return r;
        }

        template<std::size_t C>
        float Dot(const Vector<C, T>& v) const {
            static_assert(Columns == C, "Dot product requires vectors with the same dimensions");

            float r = 0.0f;
            for (std::size_t i = 0; i < Columns; ++i) {
                r += values[i] * v.values[i];
            }

            return r;
        }

        template<std::size_t D = Columns, typename std::enable_if_t<D == 3, int> = 0>
        Vector<3, T> Cross(const Vector<3, T>& v) const {
            Vector<3, T> r;
            r.values[0] = values[1] * v.values[2] - values[2] * v.values[1];
            r.values[1] = values[2] * v.values[0] - values[0] * v.values[2];
            r.values[2] = values[0] * v.values[1] - values[1] * v.values[0];
            return r;
        }

        template<std::size_t C>
        Vector<C, T> Clamp(const Vector<C, T>& min, const Vector<C, T>& max) const {
            Vector<C, T> r;
            for (std::size_t i = 0; i < Columns; ++i) {
                if (values[i] < min[i]) r.values[i] = min[i];
                else if (values[i] > max[i]) r.values[i] = max[i];
                else r.values[i] = values[i];
            }
            return r;
        }

        template<std::size_t C>
        Vector Min(const Vector<C, T>& v) const {
            Vector r;
            for (size_t i = 0; i < Columns; ++i) {
                r.values[i] = (values[i] < v.values[i]) ? values[i] : v.values[i];
            }

            return r;
        }

        template<std::size_t C>
        Vector Max(const Vector<C, T>& v) const {
            Vector r;
            for (size_t i = 0; i < Columns; ++i) {
                r.values[i] = (values[i] > v.values[i]) ? values[i] : v.values[i];
            }

            return r;
        }

        template<std::size_t C>
        Vector Sign(const Vector<C, T>& v) const {
            Vector<C, T> r;

            for (std::size_t i = 0; i < Columns; ++i) {
                float dot = this->operator[](i) * v[i];
                if (dot > 0) {
                    r[i] = 1.0f;
                } else if (dot < 0) {
                    r[i] = -1.0f;
                } else {
                    r[i] = 0.0f;
                }
            }

            return r;
        }

        template<std::size_t C>
        float Angle(const Vector<C, T>& v, Unit unit = Unit::Radians) {
            float dot = Dot(v);

            float theta = dot / (Magnitude() * v.Magnitude());
            theta = std::clamp(theta, -1.0f, 1.0f); // Todo: Use own clamp function

            float r = std::acos(theta); // result in radians

            if (unit == Unit::Degrees) {
                return r * (180.0f / M_PI);
            }

            return r;
        }

        template<std::size_t C>
        Vector Project(const Vector<C, T>& v) {
            Vector<Columns, T> r;

            T scalar = Dot(v) / v.Dot(v);
            for (std::size_t i = 0; i < Columns; ++i) {
                r[i] = scalar * v[i];
            }

            return r;
        }
    };

    using Vec = Vector<1>;
    using Vec2 = Vector<2>;
    using Vec3 = Vector<3>;
    using Vec4 = Vector<4>;
}

#endif//MARMALADE_VECTOR_H
