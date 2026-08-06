
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

#ifndef TILESET_H
#define TILESET_H

#include "../../application/config/config.h"
#include "../../ecs/components/rendering/tiledata.h"
#include "../../ecs/components/rendering/tilemap.h"

namespace Marmalade::Graphics {
    class TileSet : public Marmalade::Config<TileSetData> {
    public:
        TileSet();
        explicit TileSet(const std::filesystem::path& path, const std::string& name);

        void PrepareNewConfig() override;

        unsigned int texture = 0;
        std::vector<Tile> tiles;

        void AddTile(const Tile& tile) { tiles.push_back(tile); }
    };
}

#endif //TILESET_H
