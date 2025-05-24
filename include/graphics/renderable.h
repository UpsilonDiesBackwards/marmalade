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
#include "../ecs/components/light2d/light2d.h"
#include "material.h"

#include <glad/glad.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/mat4x4.hpp>

#include <cstdlib>
#include <vector>

class Entity;

/**
 * \brief A renderable is an object that can be rendered onto the screen
 */

class Renderable {
public:
    enum RenderMode {
        RenderMode_LIT,
        RenderMode_UNLIT,
        RenderMode_WIREFRAME,
    };

    /**
     * \brief The current RenderMode of the renderable, determines how it should be rendered
     * \brief Possible states: RenderMode_LIT, RenderMode_UNLIT, RenderMode_WIREFRAME
     */
    RenderMode renderMode = RenderMode::RenderMode_UNLIT;

    std::shared_ptr<Marmalade::Material::Material> material;

    Renderable(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int texture);

    /**
     * \brief Initialises the renderable
     * \brief Generates and binds buffer objects
     */
    void Initialise();

    /**
     * \brief Draws the renderable object to the screen
     * \param entity Owning entity
     * \param modelMatrix The modelmatrix of the renderable
     * \param renderTexture Render the renderable with a material texture
     */
    void Draw(Entity* entity, glm::mat4 modelMatrix, bool renderTexture);

    /**
     * \brief Applies lighting to the renderable
     * \param lights Vector of lights within the current scene
     * \param viewPos The current View Position of the Camera
     */
    void ApplyLighting(const std::vector<Marmalade::ECS::Light2D*>& lights, const glm::vec3& viewPos);

    /**
     * \brief Update and apply a new render mode for the renderable
     */
    void ApplyRenderMode();

private:
    RenderMode _previousRenderMode = RenderMode::RenderMode_WIREFRAME;

    unsigned int VAO, VBO, EBO;
    Shader shaderProgram;
    unsigned int texture;
    glm::mat4 projectionMatrix;
};

#endif
