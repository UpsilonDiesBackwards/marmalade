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

#ifndef MARMALADE_ECS_TRANSFORM_H
#define MARMALADE_ECS_TRANSFORM_H

#include <glm/vec2.hpp>
#include <glm/glm.hpp>

#include "../component.h"

#include <nlohmann/json.hpp>
#include <glm/detail/type_quat.hpp>

namespace Marmalade::ECS {
    class Transform : public Component {
    public:
        glm::vec3 pos = {0.0f, 0.0f, 0.0f};
        glm::quat rotation = {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        bool isDirty = true;

        void Apply(Entity* entity) override;
        void Display(Entity* entity) override;
        void Setup(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        Transform() {
            name = "Transform";
            isMutable = false;
            allowMultiple = false;
            categories = {"General"};
            description =
                    "Transform Component\n"
                    "Primary component for entities. Adds the ability to change the transform of an entity";
        }
    };

    REGISTER_COMPONENT(Transform);
}

#endif
