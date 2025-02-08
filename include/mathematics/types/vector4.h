
#ifndef MARMALADE_VECTOR4_H
#define MARMALADE_VECTOR4_H

#include <string>

namespace Marmalade::Mathematics {
    struct Vector4 {
    public:
        float x, y, z, w;
        
        Vector4(float x, float y, float z, float(w)) : x(x), y(y), z(z), w(w) { };

        Vector4 operator+(Vector4 const& vec) const;
        Vector4 operator-(Vector4 const& vec) const;
        Vector4 operator*(Vector4 const& vec) const;
        Vector4 operator/(Vector4 const& vec) const;

        float Magnitude();

        std::string ToString();
    };
}


#endif//MARMALADE_VECTOR4_H
