
#include "mathematics/types/matrix2x2.h"

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
