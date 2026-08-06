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

#include "billboardtexture.h"

#include "../material.h"
#include "../meshdata.h"

#include <glad/glad.h>

#include "../../application/util.h"

BillboardTexture::BillboardTexture(const std::string& filePath, Shader* shader) : _shader(shader) {
    setupMesh();
    loadTexture(filePath);
}
void BillboardTexture::SetPosition(const glm::vec3& pos) {
    _position = pos;
}

void BillboardTexture::Draw(const glm::mat4& view, const glm::mat4& projection) {
    _shader->Use();
    _shader->SetMat4("view", view);
    _shader->SetMat4("proj", projection);
    _shader->SetVec3("pos", _position);

    _shader->SetInt("icon", 0);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void BillboardTexture::loadTexture(const std::string& filePath) {
    Marmalade::Material::TextureSettings settings{};
    settings.wrapS = GL_CLAMP_TO_EDGE;
    settings.wrapT = GL_CLAMP_TO_EDGE;
    settings.minFilter = GL_NEAREST;
    settings.magFilter = GL_NEAREST;

    _texture = Texture::LoadTexture(filePath, settings);
    if (_texture == 0) {
        LOG_WARN("BillboardTexture failed to load: %s", filePath.c_str());
    }
}

void BillboardTexture::setupMesh() {
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glBufferData(GL_ARRAY_BUFFER, BILLBOARD_2D_RECT.size() * sizeof(float), BILLBOARD_2D_RECT.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
