
#ifndef MARMALADE_MATRIX2X2_H
#define MARMALADE_MATRIX2X2_H

#include <string>

namespace Marmalade::Mathematics {
    struct Matrix2x2 {
        float m[2][2];

        Matrix2x2() : m{{1, 0}, {0, 1}} { } // identity matrix
        Matrix2x2(float a, float b, float c, float d) : m{{a, b}, {c, d}} { }

        Matrix2x2 operator+(const Matrix2x2& mA) const;
        Matrix2x2 operator-(const Matrix2x2& mA) const;
        Matrix2x2 operator*(const Matrix2x2& mA) const;
        Matrix2x2 operator/(const Matrix2x2& mA) const;

        std::string ToString();
    };
}

#endif//MARMALADE_MATRIX2X2_H
