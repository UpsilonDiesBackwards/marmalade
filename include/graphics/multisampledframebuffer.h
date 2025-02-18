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

class MultiSampledFramebuffer {
public:
    int width = 1920;
    int height = 1080;

    ImVec2 position;

    int sampleCount = 16; // MSAA sample count. not a const because we want this to be editable by user

    MultiSampledFramebuffer();
    ~MultiSampledFramebuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    void Resolve();

    unsigned int GetTexture() const;
    unsigned int GetFBO() const;
private:
    unsigned int fbo, resolvedFBO;
    unsigned int texture, resolvedTexture;
    unsigned int rbo;

    void Initialize();
};

#endif
