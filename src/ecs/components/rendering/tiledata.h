
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

#ifndef MARMALADE_TILEDATA_H
#define MARMALADE_TILEDATA_H

#define TILESET_VERSION 1

#include <glm/glm.hpp>

#include <string>

struct Tile {
    unsigned int id = 0;
    glm::vec2 uvOffset = {0, 0};
    glm::vec2 uvSize = {1, 1};
    unsigned int texture = 0;
};

struct TextureAtlas {
    glm::uvec2 size = {0, 0}; // Size of atlas texture
    unsigned int texture = 0;
};

struct TileData {
    std::array<uint32_t, 2> position;
    std::array<uint32_t, 2> size;
    unsigned int texture = 0;
};

struct TextureAtlasData {
    std::string path;
    std::array<uint32_t, 2> size;
    unsigned int texture = 0;
};

struct TileSetData {
    std::string type = "Marmalade::Rendering::Tileset";
    int version = TILESET_VERSION;

    std::string name;
    TextureAtlasData atlas;
    std::vector<TileData> tiles;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileData, position, size, texture)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextureAtlasData, path, size)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileSetData, type, version, name, atlas, tiles)

#endif
