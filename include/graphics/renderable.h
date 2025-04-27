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

#ifndef ENGINE_RENDERABLE_H
#define ENGINE_RENDERABLE_H

#include "shader.h"
#include "ecs/components/lighting2d/light2d.h"
#include "material.h"

#include <glad/glad.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/mat4x4.hpp>

#include <cstdlib>
#include <vector>

class Entity;

class Renderable {
public:
    enum RenderMode {
        Lit,
        Unlit,
        Wireframe,
    };

    RenderMode renderMode = RenderMode::Unlit;

    std::shared_ptr<Marmalade::Material::Material> material;

    Renderable(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int texture);

    void Initialise(); // Buffer Setup
    void Draw(Entity* entity, glm::mat4 modelMatrix, bool renderTexture);

    void ApplyLighting(const std::vector<Marmalade::ECS::Light2D*>& lights, const glm::vec3& viewPos);

    void ApplyRenderMode();

private:
    RenderMode _previousRenderMode = RenderMode::Wireframe;

    unsigned int VAO, VBO, EBO;
    Shader shaderProgram;
    unsigned int texture;
    glm::mat4 projectionMatrix;
};

#endif
