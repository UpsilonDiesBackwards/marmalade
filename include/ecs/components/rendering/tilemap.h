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

#ifndef MARMALADE_TILEMAP_H
#define MARMALADE_TILEMAP_H

#include <glm/glm.hpp>

#include "ecs/component.h"
#include "ecs/components/rendering/tiledata.h"
#include "graphics/shader.h"
#include "graphics/components/tileset.h"

/**
 * \brief A tileset stores the data of cells to then be rendered by the TileGrid
 */
struct TileSet {
    TextureAtlas atlas;
    std::vector<Tile> tiles;

    void AddTile(const Tile& tile) { tiles.push_back(tile); }
};

/**
 * \brief The cell container that holds a tile in the TileGrid
 */
struct TileCell {
    int tileIndex = -1;// -1 is an empty tile
    // TODO: Colliders
};

struct TileGrid {
    glm::uvec2 size = {5, 5};
    glm::uvec2 cellSize = {32, 32};

    std::vector<TileCell> cells;
    float spacing = 1;
    int order = 0; // Y-Sorting order

    TileGrid() {
        Resize(size);
    }

    void Resize(glm::uvec2 newSize) {
        size = newSize;
        cells.resize(size.x * size.y);

        for (auto& cell : cells) {
            cell.tileIndex = -1;
        }
    }

    TileCell* GetCellAt(glm::uvec2 coords) {
        if (coords.x >= size.x || coords.y >= size.y) { return nullptr; }
        size_t index = coords.y * size.x + coords.x;
        return &cells[index];
    }

    const TileCell* GetCellAt(glm::uvec2 coords) const {
        if (coords.x >= size.x || coords.y >= size.y) { return nullptr; }
        size_t index = coords.y * size.x + coords.x;
        return &cells[index];
    }
};

namespace Marmalade::Graphics {
    class TileSet;
}

namespace Marmalade::ECS {
    class Tilemap : public Component {
    public:
#define TILEMAP_CTOR_BODY                           \
name = "Tilemap",                                   \
allowMultiple = true;                               \
categories = {"Rendering", "Graphics"};             \
description =                                       \
    "Adds a 2D tilemap component\n"                 \
    "Allows the creation of 2D tilemap environments";

#ifdef _MSC_VER
        Tilemap();
#else
        Tilemap() : tileSet(nullptr),
      shader(nullptr) {
            TILEMAP_CTOR_BODY;
        }
#endif

        Graphics::TileSet* tileSet;
        TileGrid tileGrid;

        void DisplayTilesetPicker();

        void RenderGrid(Entity* entity);
        void RenderTiles(Entity* entity) const;

        void PaintTile(glm::uvec2 coords, int tileIndex);
        void EraseTile(glm::uvec2 coords);

        void Display(Entity* entity) override;
        void Apply(Entity* entity) override;
        void Setup(Entity* entity) override;
        nlohmann::json Serialize(const Entity* entity) override;
        void Deserialize(nlohmann::json json, Entity* entity) override;

    private:
        Shader* shader;

        int selectedTile = 0;

        struct TileVertex {
            glm::vec2 position;
            glm::vec2 uv;
        };

        struct TileInstance {
            glm::vec2 worldPos;
            glm::vec2 uvOffset;
            glm::vec2 uvSize;
        };

        unsigned int tileVAO = 0;
        unsigned int tileVBO = 0;
        unsigned int tileEBO = 0;
        unsigned int instanceVBO = 0;

        void LoadTextureAtlas();

        void SetupTileQuad();
        void SetupTileInstancing();
    };

    REGISTER_COMPONENT(Tilemap);
}

#endif //TILEMAP_H