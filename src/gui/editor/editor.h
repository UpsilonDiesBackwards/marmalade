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

#ifndef ENGINE_EDITORVIEWS_H
#define ENGINE_EDITORVIEWS_H

#include <imgui.h>

#include <glm/glm.hpp>

/**
 * \brief Creates the window that displays the two editor view subwindows.
 */
class EditorViews {
public:
    void Show();

    /**
     * \brief Converts a position from world-space into screen-space coordinates
     * \param world World position
     * \return ImVec2 Screen-space position
     */
    static ImVec2 WorldToScreenSpace(const glm::vec3& world);

    /**
     * \brief Converts a position from screen-space into world-space coordinates
     * \param screen Screen position
     * \return glm::vec2 World-space position
     */
    static glm::vec2 ScreenToWorldSpace(const ImVec2& screen);
};

#endif
