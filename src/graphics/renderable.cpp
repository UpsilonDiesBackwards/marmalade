/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <graphics/renderable.h>

#include <glad/glad.h>

#include "../application/application.h"
#include "graphics/texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>

float vertices[] = {
        // Positions       // Texture Coords
        0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  // Top-left
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
        1.0f,  0.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
        0.0f,  0.0f, 0.0f,  0.0f, 0.0f   // Bottom-left
};

int indices[] = {
        0, 1, 3,
        1, 2, 3
};

Renderable::Renderable(GLuint VAO, GLuint VBO, GLuint EBO, GLuint texture) : VAO(VAO), VBO(VBO), EBO(EBO),
                        texture(texture),
                        shaderProgram(Shader("res/shaders/shader.vert", "res/shaders/shader.frag")) {
}

void Renderable::Initialise() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderable::Draw(glm::mat4 modelMatrix, bool renderTexture) {
    if (!renderTexture || texture == 0) { return; }

    shaderProgram.Use();

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shaderProgram.SetInt("texture0", 0);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    shaderProgram.SetMat4("projection", Application::GetInstance().camera->GetProjection());

    shaderProgram.SetMat4("view", Application::GetInstance().camera->GetView());
    shaderProgram.SetMat4("model", modelMatrix);

    glDrawElements(GL_TRIANGLES, sizeof(indices)/4, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderable::SetTexture(const std::string& filePath) {
    glDeleteTextures(1, &texture);

    texture = Texture::LoadTexture(filePath);
}

unsigned int Renderable::GetTexture() {
    return texture;
}
