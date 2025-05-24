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

#ifndef MARMALADE_ECS_TILEMAP_H
#define MARMALADE_ECS_TILEMAP_H

#include "vec2.hpp"
#include "ecs/component.h"

namespace Marmalade::ECS {
    class TileMap : public Component {
        struct Grid {
            // vector of tiles
            glm::vec2 size = { 10, 10 };
            float spacing = 1.0f;
        };

    public:
        Grid TileGrid;

        void RenderGUIGrid(Entity* entity);

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

        TileMap() {
            name = "TileMap";
            allowMultiple = true;
            categories = {"Rendering", "Graphics"};
            description =
                    "Adds a 2D tilemap component\n"
                    "Allows the creation of 2D tilemap environments";
        }
    };

    REGISTER_COMPONENT(TileMap);
}

#endif //MARMALADE_ECS_TILEMAP_H
