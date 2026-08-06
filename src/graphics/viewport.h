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

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <glm/detail/type_vec1.hpp>
#include <glm/fwd.hpp>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_mat4x4.hpp>

class Camera {
public:
    enum ViewportMode {
        ViewportMode_ORTHOGRAPHIC,
        ViewportMode_PERSPECTIVE,
        ViewportMode_TOP,
        ViewportMode_BOTTOM,
        ViewportMode_LEFT,
        ViewportMode_RIGHT,
    };

    glm::vec3 position3D = {0.0f, 0.0f, 5.0f};

    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    glm::vec3 right = {1.0f, 0.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    Camera(float width, float height, float zoom = 0.0f);

    void Move(float xOffset, float yOffset, bool rawWorldUnits = false);
    void Zoom(float amount);

    void UpdateViewport(float width, float height);

    glm::mat4 GetProjection();
    glm::mat4 GetView();

    glm::vec2 GetPosition() const;
    float GetZoom() const;

    void SetViewportMode(ViewportMode targetMode);
    ViewportMode GetViewportMode() const;

    void UpdateVectors();

private:
    ViewportMode viewportMode = ViewportMode_ORTHOGRAPHIC;

    glm::vec2 position;
    float zoom;
    float width, height;

    glm::mat4 viewMatrix, projectionMatrix;

    float _panSpeed = 0.3f;

    float _zoomMin = 0.01f;
    float _zoomMax = 2500.0f;

    float fov = 45.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;
};

#endif
