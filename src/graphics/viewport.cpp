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
        position.x += xOffset * panSpeed * Application::GetInstance().time.GetDeltaTime();
        position.y += yOffset * panSpeed * Application::GetInstance().time.GetDeltaTime();
    }
}

void Camera::Zoom(float amount) {
    float zoomFactor = 1.025f; // Set to <1.0 for natural scrolling
    if (amount > 0) {
        zoom *= zoomFactor;
    } else if (amount < 0) {
        zoom /= zoomFactor;
    }

    zoom = glm::clamp(zoom, 0.02f, 200.0f);
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

glm::mat4 Camera::GetView() {
    glm::mat4 view_mat = glm::mat4(1.0f);

    view_mat = glm::translate(view_mat, glm::vec3(-Application::GetInstance().camera->GetPosition(), 0.0f));
//    view_mat = glm::scale(view_mat, glm::vec3(Application::GetInstance().camera->GetZoom(), Application::GetInstance().camera->GetZoom(), 1.0f));

    return view_mat;
}


glm::vec2 Camera::GetPosition() const {
    return position;
}

float Camera::GetZoom() const {
    return zoom;
}
