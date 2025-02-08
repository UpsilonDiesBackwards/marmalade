
#ifndef MARMALADE_VECTOR2_H
#define MARMALADE_VECTOR2_H

#include <string>

namespace Marmalade::Mathematics {
    struct Vector2 {
    public:
        float x, y;

        Vector2(float x, float y) : x(x), y(y) { };

        Vector2 operator+(Vector2 const& vec) const;
        Vector2 operator-(Vector2 const& vec) const;
        Vector2 operator*(Vector2 const& vec) const;
        Vector2 operator/(Vector2 const& vec) const;

        float Magnitude();

        std::string ToString();
    };
}

#endif//MARMALADE_VECTOR2_H
