
#include "mathematics/types/vector4.h"

#include <cmath>
#include <format>
#include <string>

using namespace Marmalade::Mathematics;

Vector4 Vector4::operator+(const Vector4& vec) const {
    return Vector4(vec.x+x, vec.y+y, vec.z+z, vec.w+w);
}
Vector4 Vector4::operator-(const Vector4& vec) const {
    return Vector4(vec.x-x, vec.y-y, vec.z-z, vec.w-w);
}
Vector4 Vector4::operator*(const Vector4& vec) const {
    return Vector4(vec.x*x, vec.y*y, vec.z*z, vec.w*w);
}
Vector4 Vector4::operator/(const Vector4& vec) const {
    return Vector4(vec.x/x, vec.y/y, vec.z/z, vec.w/w);
}
float Vector4::Magnitude() {
    return sqrt(x*x + y*y + z*z + w*w);
}

std::string Vector4::ToString() {
    return std::format("{}, {}, {}, {}", x, y, z, w);
}