
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

#ifndef ENGINE_GAMEVIEW_H
#define ENGINE_GAMEVIEW_H


#include "../../graphics/multisampledframebuffer.h"
#include "imgui.h"

class GameView {
/**
 * \brief Game view of the application
 * \brief This is the main game view that shows the state of the game as-is
 */
public:
    GameView(int width, int height);
    ~GameView();

    /**
     * \brief Render the game view.
     */
    void Render();

    /**
     * \brief Resize the game viewport to a specified dimension
     * \param width Desired width of the viewport
     * \param height Desired height of the viewport
     */
    void Resize(int width, int height);

    /**
     * \brief Execute any game viewport-specific input
     * \todo This eventually ought to be developer-specified controls that are created via the API
     */
    void RunInput();
private:
    MultiSampledFramebuffer framebuffer;
    int width, height;
    ImVec2 imageMin, imageMax;
};


#endif //ENGINE_GAMEVIEW_H
