
#ifndef MARMALADE_VECTOR3_H
#define MARMALADE_VECTOR3_H

#include <string>

namespace Marmalade::Mathematics {
    struct Vector3 {
    public:
        float x, y, z;

        Vector3(float x, float y, float z) : x(x), y(y), z(z) { };

        Vector3 operator+(Vector3 const& vec) const;
        Vector3 operator-(Vector3 const& vec) const;
        Vector3 operator*(Vector3 const& vec) const;
        Vector3 operator/(Vector3 const& vec) const;

        float Magnitude();

        std::string ToString();
    };
}


#endif//MARMALADE_VECTOR3_H
