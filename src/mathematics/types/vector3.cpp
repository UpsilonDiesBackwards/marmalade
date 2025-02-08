
#include "mathematics/types/vector3.h"

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
