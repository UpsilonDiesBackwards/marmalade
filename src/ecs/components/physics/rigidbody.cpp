
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

#include "ecs/components/physics/rigidbody.h"

#include <imgui.h>

void Marmalade::ECS::RigidBody::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    ImGui::Checkbox("Static", &isStatic);
    ImGui::DragFloat2("Velocity", &velocity.x, 0.1f);
    ImGui::DragFloat("Gravity", &gravity, 0.1f);
    ImGui::DragFloat("Mass", &mass, 0.1f);
}

void Marmalade::ECS::RigidBody::Apply(Entity* entity) {
    if (isStatic) return;

    float deltaTime = static_cast<float>(Application::GetInstance().profiler.GetDeltaTime());

    _accumulator += deltaTime;

    while (_accumulator >= fixedTimeStep) { // We use an accumulator to prevent jitteriness
        UpdatePhysics(entity, fixedTimeStep);
        _accumulator -= fixedTimeStep;
    }
}

void Marmalade::ECS::RigidBody::UpdatePhysics(Entity* entity, float deltaTime) {
    velocity.y += gravity * deltaTime;

    glm::vec2 newPos = entity->getPosition() + velocity * deltaTime;
    entity->setPosition(newPos);
}
