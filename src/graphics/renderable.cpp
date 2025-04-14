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

#define OPENGL_VERSION "430"

float vertices[] = {
        // Position         // Normal          // UV
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom Right
        0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top Right
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f  // Top Left
};


unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
};

Renderable::Renderable(GLuint VAO, GLuint VBO, GLuint EBO, GLuint texture) : VAO(0), VBO(0), EBO(0),
                                                                             texture(texture),
                                                                             shaderProgram(Shader("res/shaders/opengl/" OPENGL_VERSION "/shader.vert", "res/shaders/opengl/" OPENGL_VERSION "/shader.frag")) {

    ApplyRenderMode();
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

    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); // Texture Coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderable::Draw(Entity* entity, glm::mat4 modelMatrix, bool renderTexture) {
    if (!renderTexture || texture == 0) { return; }

    shaderProgram.Use();

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shaderProgram.SetInt("texture0", 0);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);

    shaderProgram.SetMat4("projection", Application::GetInstance().camera->GetProjection());

    shaderProgram.SetMat4("view", Application::GetInstance().camera->GetView());
    shaderProgram.SetMat4("model", modelMatrix);

    shaderProgram.SetVec3("viewPos", glm::vec3(Application::GetInstance().camera->GetPosition(), 1.0f));

    ApplyRenderMode();

    for (int i = 0; i < Application::GetInstance().sceneManager.GetCurrentScene()->GetLights().size(); ++i) {
        auto light = Application::GetInstance().sceneManager.GetCurrentScene()->GetLights()[i];
        std::string baseName = "lights[" + std::to_string(i) + "]";

        shaderProgram.SetVec3(baseName + ".position", glm::vec3(light->GetPosition(), 1.0f));

        shaderProgram.SetFloat(baseName + ".intensity", light->intensity);
        shaderProgram.SetVec3(baseName + ".color", light->color);
        shaderProgram.SetFloat(baseName + ".radius", light->radius);
        shaderProgram.SetFloat(baseName + ".attenuation", light->attenuation);

    }
    shaderProgram.SetInt("numLights", Application::GetInstance().sceneManager.GetCurrentScene()->GetLights().size());

    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);

    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
}

void Renderable::ApplyRenderMode() {
    if (_previousRenderMode == renderMode) { return; }

    switch (renderMode) {
        case RenderMode::Lit:
            shaderProgram.SetBool("useLighting", true);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RenderMode::Unlit:
            shaderProgram.SetBool("useLighting", false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RenderMode::Wireframe:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
    }

    _previousRenderMode = renderMode;
}

void Renderable::UpdateTextureSettings() {
    if (texture == 0) return;

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texSettings.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texSettings.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texSettings.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texSettings.magFilter);
}

void Renderable::SetTexture(const std::string& filePath) {
    texSettings.filePath = filePath;

    glDeleteTextures(1, &texture);

    texture = Texture::LoadTexture(filePath);

    UpdateTextureSettings();
}

unsigned int Renderable::GetTexture() {
    return texture;
}