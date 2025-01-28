#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "../../include/archetypes/camera.h"
#include "../application/application.h"

Camera::Camera(float width, float height, float zoom) : position(0.0f, 0.0f), zoom(zoom), width(width), height(height) {
}

void Camera::Move(float xOffset, float yOffset) {
    position.x += xOffset * panSpeed * Application::GetInstance().profiler.GetDeltaTime();
    position.y += yOffset * panSpeed * Application::GetInstance().profiler.GetDeltaTime();
}

void Camera::Zoom(float amount) {
    zoom += amount * Application::GetInstance().profiler.GetDeltaTime();
    if (zoom < -100.0f) zoom = -100.0f;
    if (zoom > 100.0f) zoom = 100.0f;
}

void Camera::UpdateViewport(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

glm::mat4 Camera::GetProjection() {
    float left = -10.0f;
    float right = 10.0f;
    float bottom = -10.0f;
    float top = 10.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::mat4 projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    glm::mat4 projection = glm::ortho(0.0f, 10.0f, 10.0f, 0.0f, -1.0f, 1.0f);

    return projection;
}


glm::vec2 Camera::GetPosition() const {
    return position;
}

float Camera::GetZoom() const {
    return zoom;
}
