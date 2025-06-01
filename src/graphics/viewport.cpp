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

#include <graphics/viewport.h>

#include "../application/application.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float width, float height, float zoom) : position(0.0f, 0.0f), zoom(zoom), width(width), height(height) {
}

void Camera::Move(float xOffset, float yOffset, bool rawWorldUnits) {
    if (rawWorldUnits) {
        position.x += xOffset;
        position.y += yOffset;
    } else {
        position.x += xOffset * _panSpeed * Application::GetInstance().time.GetDeltaTime();
        position.y += yOffset * _panSpeed * Application::GetInstance().time.GetDeltaTime();
    }
}

void Camera::Zoom(float amount) {
    float zoomFactor = 1.025f; // Set to <1.0 for natural scrolling
    if (amount > 0) {
        zoom *= zoomFactor;
    } else if (amount < 0) {
        zoom /= zoomFactor;
    }

    zoom = glm::clamp(zoom, _zoomMin, _zoomMax);
}

void Camera::UpdateViewport(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

glm::mat4 Camera::GetProjection() {
    switch (viewportMode) {
        case ViewportMode_PERSPECTIVE:
            return glm::perspective(glm::radians(fov), width / height, nearClip, farClip);

        case ViewportMode_ORTHOGRAPHIC:
        case ViewportMode_TOP:
        case ViewportMode_BOTTOM:
        case ViewportMode_LEFT:
        case ViewportMode_RIGHT: {
            float orthoWidth = width * zoom;
            float orthoHeight = height * zoom;
            return glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -orthoHeight / 2.0f, orthoHeight / 2.0f, -farClip, farClip);
        }

        default:
            return glm::mat4(1.0f);
    }
}

glm::mat4 Camera::GetView() {
    switch (viewportMode) {
        case ViewportMode_PERSPECTIVE:
            return glm::lookAt(position3D, position3D + front, up);
        case ViewportMode_TOP:
            return glm::lookAt(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        case ViewportMode_BOTTOM:
            return glm::lookAt(glm::vec3(0, -10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
        case ViewportMode_LEFT:
            return glm::lookAt(glm::vec3(-10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        case ViewportMode_RIGHT:
            return glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        case ViewportMode_ORTHOGRAPHIC:
        default:
            return glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
    }
}

glm::vec2 Camera::GetPosition() const {
    return position;
}

float Camera::GetZoom() const {
    return zoom;
}

void Camera::SetViewportMode(ViewportMode targetMode) {
    viewportMode = targetMode;

    if (viewportMode != ViewportMode_PERSPECTIVE) {
        position = {0.0f, 0.0f};
        zoom = 1.0f;
    }
}

Camera::ViewportMode Camera::GetViewportMode() const {
    return viewportMode;
}

void Camera::UpdateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}
