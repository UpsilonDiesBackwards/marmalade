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

#ifndef MARMALADE_ECS_MATERIALRENDER_H
#define MARMALADE_ECS_MATERIALRENDER_H

#include "../../component.h"

#include "../../../graphics/renderable.h"

#include <nlohmann/json.hpp>

namespace Marmalade::ECS {
    class MaterialRenderer : public Component {
    public:
        std::filesystem::path materialPath;

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;

        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        nlohmann::json SerializeTexture(const Marmalade::Material::Texture& texture);
        void DeserializeTexture(const nlohmann::json& textureJson, Marmalade::Material::Texture& texture);

        MaterialRenderer() {
            name = "Material Renderer";
            allowMultiple = false;
            categories = {"General"};
            description =
                    "Adds a material renderer component\n"
                    "Allows the assignment of albedo, normal, and other types of texture maps to an entity.";
        }
    };

    REGISTER_COMPONENT(MaterialRenderer);
}

#endif
