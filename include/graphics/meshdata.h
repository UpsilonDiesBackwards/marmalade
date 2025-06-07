// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_MESHDATA_H
#define MARMALADE_MESHDATA_H

#define BILLBOARD_2D_RECT                      \
    std::vector<float>{                        \
            /* positions         tex coords */ \
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,     \
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    \
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,     \
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,     \
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,     \
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f}      \

#define SPRITE_2D_RECT \
    std::vector<float>{ \
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, \
    0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, \
    0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, \
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f \
    }, \
    std::vector<unsigned int>{0,1,2, 2,3,0}

#define CUBE_MESH \
    std::tuple { \
    std::vector<float>{ \
        /* Front face */ \
        -0.5f, -0.5f,  0.5f,   0,  0,  1,   0, 0, \
         0.5f, -0.5f,  0.5f,   0,  0,  1,   1, 0, \
         0.5f,  0.5f,  0.5f,   0,  0,  1,   1, 1, \
        -0.5f, -0.5f,  0.5f,   0,  0,  1,   0, 0, \
         0.5f,  0.5f,  0.5f,   0,  0,  1,   1, 1, \
        -0.5f,  0.5f,  0.5f,   0,  0,  1,   0, 1, \
        /* Back face */ \
         0.5f, -0.5f, -0.5f,   0,  0, -1,   0, 0, \
        -0.5f, -0.5f, -0.5f,   0,  0, -1,   1, 0, \
        -0.5f,  0.5f, -0.5f,   0,  0, -1,   1, 1, \
         0.5f, -0.5f, -0.5f,   0,  0, -1,   0, 0, \
        -0.5f,  0.5f, -0.5f,   0,  0, -1,   1, 1, \
         0.5f,  0.5f, -0.5f,   0,  0, -1,   0, 1, \
        /* Left face */ \
        -0.5f, -0.5f, -0.5f,  -1,  0,  0,   0, 0, \
        -0.5f, -0.5f,  0.5f,  -1,  0,  0,   1, 0, \
        -0.5f,  0.5f,  0.5f,  -1,  0,  0,   1, 1, \
        -0.5f, -0.5f, -0.5f,  -1,  0,  0,   0, 0, \
        -0.5f,  0.5f,  0.5f,  -1,  0,  0,   1, 1, \
        -0.5f,  0.5f, -0.5f,  -1,  0,  0,   0, 1, \
        /* Right face */ \
         0.5f, -0.5f,  0.5f,   1,  0,  0,   0, 0, \
         0.5f, -0.5f, -0.5f,   1,  0,  0,   1, 0, \
         0.5f,  0.5f, -0.5f,   1,  0,  0,   1, 1, \
         0.5f, -0.5f,  0.5f,   1,  0,  0,   0, 0, \
         0.5f,  0.5f, -0.5f,   1,  0,  0,   1, 1, \
         0.5f,  0.5f,  0.5f,   1,  0,  0,   0, 1, \
        /* Top face */ \
        -0.5f,  0.5f,  0.5f,   0,  1,  0,   0, 0, \
         0.5f,  0.5f,  0.5f,   0,  1,  0,   1, 0, \
         0.5f,  0.5f, -0.5f,   0,  1,  0,   1, 1, \
        -0.5f,  0.5f,  0.5f,   0,  1,  0,   0, 0, \
         0.5f,  0.5f, -0.5f,   0,  1,  0,   1, 1, \
        -0.5f,  0.5f, -0.5f,   0,  1,  0,   0, 1, \
        /* Bottom face */ \
        -0.5f, -0.5f, -0.5f,   0, -1,  0,   0, 0, \
         0.5f, -0.5f, -0.5f,   0, -1,  0,   1, 0, \
         0.5f, -0.5f,  0.5f,   0, -1,  0,   1, 1, \
        -0.5f, -0.5f, -0.5f,   0, -1,  0,   0, 0, \
         0.5f, -0.5f,  0.5f,   0, -1,  0,   1, 1, \
        -0.5f, -0.5f,  0.5f,   0, -1,  0,   0, 1 \
    }, \
    std::vector<unsigned int>{ \
    0, 1, 2, 3, 4, 5, \
    6, 7, 8, 9, 10, 11, \
    12, 13, 14, 15, 16, 17, \
    18, 19, 20, 21, 22, 23, \
    24, 25, 26, 27, 28, 29, \
    30, 31, 32, 33, 34, 35 \
    } \
}

#endif