
#include <format>
#include <cmath>

#include "../../../include/mathematics/types/vector2.h"

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
