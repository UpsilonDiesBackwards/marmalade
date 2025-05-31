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

#include "../include/ecs/components/physics/2d/boxcollider2d.h"

#include "ecs/components/physics/2d/rigidbody2d.h"
#include "scene/entity.h"

#include "physics/2d/collider2d.h"

#ifdef _MSC_VER
Marmalade::ECS::BoxCollider2D::BoxCollider2D() {
    BOXCOLLIDER2D_CTOR_BODY
}
#endif

void Marmalade::ECS::BoxCollider2D::Display(Entity* entity) {}

void Marmalade::ECS::BoxCollider2D::Apply(Entity* entity) {}

void Marmalade::ECS::BoxCollider2D::Setup(Entity* entity) {
    if (auto rb = entity->componentManager.GetComponentOfType<Rigidbody2D>()) {
        rb->body.collider.type = Physics::ColliderType2D::Box;

        auto shape = std::make_shared<Physics::BoxCollider2D>();
        shape->size = this->size;
        shape->offset = this->offset;

        rb->body.collider.shape = shape;
    }
}

nlohmann::json Marmalade::ECS::BoxCollider2D::Serialize(const Entity* entity) {
    nlohmann::json j;
    return j;
}

void Marmalade::ECS::BoxCollider2D::Deserialize(nlohmann::json json, Entity* entity) {}