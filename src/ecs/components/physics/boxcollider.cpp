
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

#include "ecs/components/physics/boxcollider.h"

#include "../../../application/application.h"

#include <imgui.h>

void Marmalade::ECS::BoxCollider::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::DragFloat2("Size", &size.x, 0.1);
    ImGui::DragFloat2("Offset", &offset.x, 0.1);

    ImGui::Checkbox("Draw bounds", &drawBounds);

    if (drawBounds) {
        DrawBounds(entity);
    }
}

void Marmalade::ECS::BoxCollider::Apply(Entity* entity) {
    for (auto other : Application::GetInstance().sceneManager.GetCurrentScene()->GetEntities()) {
        if (other.get() == entity) { continue; }

        if (!other->componentManager.GetComponentOfType<BoxCollider>()) { return; }

        glm::vec2 posA = entity->getPosition();
        glm::vec2 posB = other->getPosition();

        if (Intersects(*other->componentManager.GetComponentOfType<BoxCollider>(), posA, posB)) {
            spdlog::info("Entity collision! {} with {}", entity->name, other->name);
        }
    }
}

bool Marmalade::ECS::BoxCollider::Intersects(const Marmalade::ECS::BoxCollider& other, const glm::vec2& posA, const glm::vec2& posB) const {
    glm::vec2 minA = posA + offset;
    glm::vec2 maxA = minA + size;

    glm::vec2 minB = posB + other.offset;
    glm::vec2 maxB = minB + other.size;

    return (minA.x < maxB.x && maxA.x > minB.x &&
            minA.y < maxB.y && maxA.y > minB.y);
}

void Marmalade::ECS::BoxCollider::DrawBounds(Entity* entity) {
    // Todo: Draw bounding box
}

