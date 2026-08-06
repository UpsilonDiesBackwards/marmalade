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

#ifndef MARMALADE_BILLBOARDTEXTURE_H
#define MARMALADE_BILLBOARDTEXTURE_H

#include "../shader.h"

#include <glm/glm.hpp>
#include <string>

class BillboardTexture {
public:
    BillboardTexture(const std::string& filePath, Shader* shader);

    void SetPosition(const glm::vec3& pos);
    void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int _VAO = 0, _VBO = 0, _texture = 0;
    glm::vec3 _position;
    Shader* _shader;

    void loadTexture(const std::string& filePath);
    void setupMesh();
};



#endif //MARMALADE_BILLBOARDTEXTURE_H
