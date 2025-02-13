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

Camera::Camera(float width, float height, float zoom) : position(0.0f, 0.0f), zoom(zoom), width(width), height(height) {
}

void Camera::Move(float xOffset, float yOffset) {
    position.x += xOffset * panSpeed * Application::GetInstance().profiler.GetDeltaTime();
    position.y += yOffset * panSpeed * Application::GetInstance().profiler.GetDeltaTime();
}

void Camera::Zoom(float amount) {
    zoom += amount * Application::GetInstance().profiler.GetDeltaTime();
    if (zoom <= 0.02f) zoom = 0.02f;
    if (zoom > 200.0f) zoom = 200.0f;
}

void Camera::UpdateViewport(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

glm::mat4 Camera::GetProjection() {
    float aspectRatio = width / height;
    float orthoSize = 10.0f / zoom;

    glm::mat4 projection = glm::ortho(
            -orthoSize * aspectRatio, orthoSize * aspectRatio,
            -orthoSize, orthoSize,
            -1.0f, 1.0f
    );

    return projection;
}

glm::vec2 Camera::GetPosition() const {
    return position;
}

float Camera::GetZoom() const {
    return zoom;
}
