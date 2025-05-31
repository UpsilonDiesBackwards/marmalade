// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "../include/ecs/components/physics/2d/circlecollider2d.h"

#include "ecs/components/physics/2d/rigidbody2d.h"
#include "scene/entity.h"

#ifdef _MSC_VER
Marmalade::ECS::CircleCollider2D::CircleCollider2D() {
    CIRCLECOLLIDER2D_CTOR_BODY
}
#endif

void Marmalade::ECS::CircleCollider2D::Display(Entity* entity) {}

void Marmalade::ECS::CircleCollider2D::Apply(Entity* entity) {}

void Marmalade::ECS::CircleCollider2D::Setup(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        rb->body.collider.type = Physics::ColliderType2D::Circle;

        auto shape = std::make_shared<Physics::CircleCollider2D>();
        shape->offset = this->offset;
        shape->offset = this->offset;

        rb->body.collider.shape = shape;
    }
}

nlohmann::json Marmalade::ECS::CircleCollider2D::Serialize(const Entity* entity) {
    nlohmann::json j;
    return j;
}

void Marmalade::ECS::CircleCollider2D::Deserialize(nlohmann::json json, Entity* entity) {}