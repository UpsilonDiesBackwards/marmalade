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

#ifndef ENGINE_EDITVIEW_H
#define ENGINE_EDITVIEW_H

#include <graphics/multisampledframebuffer.h>

#include <imgui.h>
#include "scene/entity.h"

class EditView {
public:
    Entity* selectedEntity{};

    EditView(int width, int height);
    ~EditView();

    void Render();
    void Resize(int width, int height);

    void RunInput();
private:
    MultiSampledFramebuffer framebuffer;
    int width, height;
    ImVec2 imageMin, imageMax;

    void ShowGizmo();
};


#endif
