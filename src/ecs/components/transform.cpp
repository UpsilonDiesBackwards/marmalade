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

#include <ecs/components/transform.h>

#include <scene/entity.h>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

void Transform::Display(Entity* entity) {
        ImGui::Text(name.c_str());

        // Transform
        if (ImGui::InputFloat2(("Position##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->pos))) {
            entity->setPosition(glm::vec2(entity->transform->pos[0], entity->transform->pos[1]));
        }
        if (ImGui::SliderFloat(("Rotation##" + std::to_string(entity->id)).c_str(), &entity->transform->rotation, 0, 360)) {
            entity->setRotation(entity->transform->rotation);
        }
        if (ImGui::InputFloat2(("Scale##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->scale))) {
            entity->setScale(glm::vec2(entity->transform->scale[0], entity->transform->scale[1]));
        }
}

void Transform::Apply(Entity* entity) {
}

bool Transform::_registered = Transform::Register();
