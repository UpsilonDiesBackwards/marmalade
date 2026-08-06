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

#ifndef ENGINE_EDITVIEW_H
#define ENGINE_EDITVIEW_H

#include "../../graphics/multisampledframebuffer.h"
#include "ImGuizmo.h"

#include "../../scene/entity.h"

#include <imgui.h>

/**
 * \brief Edit view of the application
 * \brief This is the main editor view that allows the user to manipulate entities, display guizmos and other development-specific information
 */
class EditView {
    /**
     * \brief Current Render Mode of the application as an integer
     */
    static int currentRenderMode;
    const char* renderModes[3] = { "Lit", "Unlit", "Wireframe" };

public:
    /**
     * \brief The entity that has been selected in the scene hierarchy
     */
    Entity* selectedEntity{};

    EditView(int width, int height);
    ~EditView();

    /**
     * \brief Render the edit view.
     */
    void Render();

    /**
     * \brief Resize the editor viewport to a specified dimension
     * \param width Desired width of the viewport
     * \param height Desired height of the viewport
     */
    void Resize(int width, int height);

    /**
     * \brief Execute any editor viewport-specific input such as guizmo-control hotkeys
     */
    void RunInput();
    void ViewportOrthographicInput();
    void ViewportPerspectiveInput();
private:
    MultiSampledFramebuffer framebuffer;
    int width, height;
    ImVec2 imageMin, imageMax;

    ImGuizmo::OPERATION _currentGuizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE _currentGuizmoMode = ImGuizmo::WORLD;

    /**
     * \brief Toggles the display of all UI guizmos
     */
    void ShowEditorUIGuizmos();

    /**
     * \brief Toggles display of the entity transform guizmo
     */
    void ShowGizmo();

    /**
     * \brief Toggles display of entity collider bounds
     */
    void ShowColliderBounds();

    /**
     * \brief Toggle display of bounds of the size and attenuation bounds of any Lights in the scene
     */
    void ShowLightBounds();
};

#endif
