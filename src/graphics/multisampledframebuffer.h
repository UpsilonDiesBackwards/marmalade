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

#ifndef ENGINE_MULTISAMPLEDFRAMEBUFFER_H
#define ENGINE_MULTISAMPLEDFRAMEBUFFER_H

#include <imgui.h>

/**
 * \brief Creates a multi-sampled (MS) OpenGL framebuffer
 */

class MultiSampledFramebuffer {
public:
    int width = 1920;
    int height = 1080;

    ImVec2 position;

    int sampleCount = 4;

    MultiSampledFramebuffer();
    ~MultiSampledFramebuffer();

    /**
     * \brief Bind the MS framebuffer to the current OpenGL context
     */
    void Bind();

    /**
     * \brief Unbind the MS framebuffer from the current OpenGL context
     */
    void Unbind();

    /**
     * \brief Resolve the MS framebuffer
     * \brief Resolving is the act of taking each pixel (which has multiple samples) and converting it into a singular image with 1 sample per pixel
     */
    void Resolve();

    /**
     * \brief Resize the MS framebuffer
     * \param width Desired width of the MS framebuffer
     * \param height Desired height of the MS framebuffer
     */
    void Resize(int width, int height);

    /**
     * \brief Refresh by deleting then re-initialising it as a new MS framebuffer
     */
    void Refresh();

    /**
     * \brief Returns the framebuffer texture ID
     * \return uint
     */
    unsigned int GetTexture() const;

    /**
     * \brief Returns the framebuffer object ID
     * \return uint
     */
    unsigned int GetFBO() const;
private:
    unsigned int fbo, resolvedFBO;
    unsigned int texture, resolvedTexture;
    unsigned int rbo;

    void Initialize();
};

#endif
