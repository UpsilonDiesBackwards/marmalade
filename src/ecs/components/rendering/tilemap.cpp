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

#include "tilemap.h"

#include "../src/gui/windows/stylemanager.h"

#include <imgui.h>

#include "ImGuiFileDialog.h"
#include "../../../application/application.h"
#include "../../../gui/windowmanager.h"
#include "../../../gui/components/selectablelistview.h"
#include  "../../../gui/components/backgroundlabel.h"
#include "../../../gui/editor/editor.h"
#include "../../../scene/entity.h"

#ifdef _MSC_VER
Marmalade::ECS::Tilemap::Tilemap() {
    TILEMAP_CTOR_BODY
}
#endif

void Marmalade::ECS::Tilemap::Display(Entity* entity) {
    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        bool changed = false;

        // Size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Size");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        int sizeX = static_cast<int>(tileGrid.size.x);
        if (ImGui::DragInt(("##SizeX" + std::to_string(entity->id)).c_str(), &sizeX, 1.0f, 1, 5012)) {
            tileGrid.size.x = static_cast<glm::uvec2::value_type>(sizeX);
            changed = true;
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        int sizeY = static_cast<int>(tileGrid.size.y);
        if (ImGui::DragInt(("##SizeY" + std::to_string(entity->id)).c_str(), &sizeY, 1.0f, 1, 5012)) {
            tileGrid.size.y = static_cast<glm::uvec2::value_type>(sizeY);
            changed = true;
        }
        ImGui::PopItemWidth();

        // Cell Size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Cell Size");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        int cellX = static_cast<int>(tileGrid.cellSize.x);
        if (ImGui::DragInt(("##CellX" + std::to_string(entity->id)).c_str(), &cellX, 1.0f, 1, 5012)) {
            tileGrid.cellSize.x = static_cast<glm::uvec2::value_type>(cellX);
            changed = true;
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        int cellY = static_cast<int>(tileGrid.cellSize.y);
        if (ImGui::DragInt(("##CellY" + std::to_string(entity->id)).c_str(), &cellY, 1.0f, 1, 5012)) {
            tileGrid.cellSize.y = static_cast<glm::uvec2::value_type>(cellY);
            changed = true;
        }
        ImGui::PopItemWidth();

        // Spacing
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Spacing");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        changed |= ImGui::DragFloat(("##Spacing" + std::to_string(entity->id)).c_str(), &tileGrid.spacing, 0.1f, 0.1f, 256.0f);
        ImGui::PopItemWidth();

        // Order
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Order");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Z ", COL_CATPPUCCIN_UI_BLUE);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        changed |= ImGui::DragInt(("##Order" + std::to_string(entity->id)).c_str(), &tileGrid.order, 1.0f, -100, 100);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }

    if (tileSet == nullptr) {
        ImGui::TextColored(COL_CATPPUCCIN_UI_RED, "No tile set provided");

        if (ImGui::Button("Create Tileset", ImVec2(0, 25))) {
            IGFD::FileDialogConfig config;
            config.path = GET_APP.GetCurrentProject() == nullptr ? "" : GET_APP.GetCurrentProject()->basePath.string();
            config.flags = ImGuiFileDialogFlags_Modal;

            IGFD::FileDialog::Instance()->OpenDialog(
                    "CreateTileSetDialog",
                    "Choose Texture",
                    ".png,.jpg,.jpeg,.bmp",
                    config
                    );
        }

        if (IGFD::FileDialog::Instance()->Display("CreateTileSetDialog")) {
            if (IGFD::FileDialog::Instance()->IsOk()) {
                std::string filePath = IGFD::FileDialog::Instance()->GetFilePathName();

                TextureAtlasData textureAtlasData;
                textureAtlasData.path = filePath;

                glm::uvec2 dim = Texture::GetDimensions(filePath);
                textureAtlasData.size = {dim.x, dim.y};

                unsigned int textureID = Texture::LoadTexture(filePath);

                if (!tileSet) { tileSet = new Graphics::TileSet(); }

                tileSet->storedConfig.atlas.texture = textureID;
                tileSet->storedConfig.atlas.path = filePath;
                tileSet->filePath = filePath;

                LoadTextureAtlas();
            }


            IGFD::FileDialog::Instance()->Close();
        }

        ImGui::SameLine();
        ImGui::Button("Load tileset", ImVec2(0, 25));
    }

    if (tileSet != nullptr) { if (!tileSet->filePath.empty()) { ImGui::Text("Using tileset: %s (ID: %u)", tileSet->filePath.c_str(), tileSet->storedConfig.atlas.texture); } else { ImGui::TextColored(COL_CATPPUCCIN_UI_RED, "Tileset loaded but filePath is empty"); } } else { ImGui::TextColored(COL_CATPPUCCIN_UI_RED, "No tileset loaded"); }

    if (tileSet) {
        ImGui::Separator();
        DisplayTilesetPicker();
    }
}

void Marmalade::ECS::Tilemap::Apply(Entity* entity) { RenderTiles(entity); }

void Marmalade::ECS::Tilemap::Setup(Entity* entity) {
    SetupTileQuad();
    SetupTileInstancing();

    if (!shader) {
        shader = new Shader(
                "res/shaders/opengl/430/graphical/tilemap.vert",
                "res/shaders/opengl/430/graphical/tilemap.frag"
                );
    }
}

nlohmann::json Marmalade::ECS::Tilemap::Serialize(const Entity* entity) {
    nlohmann::json j;

    if (tileSet) {
        tileSet->storedConfig.tiles.clear();
        for (const Tile& tile: tileSet->tiles) {
            TileData td;
            td.position = {
                    static_cast<uint32_t>(tile.uvOffset.x * tileSet->storedConfig.atlas.size[0]),
                    static_cast<uint32_t>(tile.uvOffset.y * tileSet->storedConfig.atlas.size[1])
            };
            td.size = {
                    static_cast<uint32_t>(tile.uvSize.x * tileSet->storedConfig.atlas.size[0]),
                    static_cast<uint32_t>(tile.uvSize.y * tileSet->storedConfig.atlas.size[1])
            };
            td.texture = 0; // Runtime only, ignore in serialization
            tileSet->storedConfig.tiles.push_back(td);
        }
        j["tileSet"] = tileSet->storedConfig;
    }

    j["gridSize"] = {{"x", tileGrid.size.x}, {"y", tileGrid.size.y}};
    j["cellSize"] = {{"x", tileGrid.cellSize.x}, {"y", tileGrid.cellSize.y}};

    std::vector<int> tilesInd;
    for (const TileCell& cell: tileGrid.cells) { tilesInd.push_back(cell.tileIndex); }
    j["tileIndices"] = tilesInd;

    return j;
}

void Marmalade::ECS::Tilemap::Deserialize(nlohmann::json json, Entity* entity) {
    if (json.contains("tileSet")) {
        if (!tileSet) tileSet = new Marmalade::Graphics::TileSet();

        tileSet->storedConfig = json["tileSet"].get<TileSetData>();
        tileSet->filePath = tileSet->storedConfig.atlas.path;
        tileSet->texture = Texture::LoadTexture(tileSet->filePath.string());

        tileSet->tiles.clear();
        for (const TileData& td: tileSet->storedConfig.tiles) {
            Tile t;
            t.id = static_cast<unsigned int>(tileSet->tiles.size());

            t.uvOffset = glm::vec2(
                    float(td.position[0]) / float(tileSet->storedConfig.atlas.size[0]),
                    float(td.position[1]) / float(tileSet->storedConfig.atlas.size[1])
                    );
            t.uvSize = glm::vec2(
                    float(td.size[0]) / float(tileSet->storedConfig.atlas.size[0]),
                    float(td.size[1]) / float(tileSet->storedConfig.atlas.size[1])
                    );
            t.texture = tileSet->texture;

            tileSet->tiles.push_back(t);
        }
    } else {
        if (tileSet) {
            delete tileSet;
            tileSet = nullptr;
        }
    }

    tileGrid.size = {
            json["gridSize"]["x"].get<int>(),
            json["gridSize"]["y"].get<int>()
    };
    tileGrid.cellSize = {
            json["cellSize"]["x"].get<int>(),
            json["cellSize"]["y"].get<int>()
    };

    tileGrid.Resize(tileGrid.size);

    if (json.contains("tileIndices") && json["tileIndices"].is_array()) {
        size_t count = std::min(json["tileIndices"].size(), tileGrid.cells.size());
        for (size_t i = 0; i < count; i++) { tileGrid.cells[i].tileIndex = json["tileIndices"][i]; }
        for (size_t i = count; i < tileGrid.cells.size(); i++) { tileGrid.cells[i].tileIndex = -1; }
    } else { for (auto& cell: tileGrid.cells) { cell.tileIndex = -1; } }

    SetupTileQuad();
    SetupTileInstancing();
}

void Marmalade::ECS::Tilemap::DisplayTilesetPicker() {
    if (!tileSet || tileSet->tiles.empty()) {
        ImGui::TextColored(COL_CATPPUCCIN_UI_RED, "No tileset loaded!");
        return;
    }

    ImGui::Text("Tileset:");
    int tilesPerRow = 8;
    float tileButtonSize = 32.0f;

    for (int i = 0; i < tileSet->tiles.size(); i++) {
        const Tile& t = tileSet->tiles[i];

        ImGui::PushID(i);
        if (ImGui::ImageButton("TilePicker",
                               (ImTextureID) (intptr_t) tileSet->texture,
                               ImVec2(tileButtonSize, tileButtonSize),
                               ImVec2(t.uvOffset.x, t.uvOffset.y),
                               ImVec2(t.uvOffset.x + t.uvSize.x, t.uvOffset.y + t.uvSize.y))) { selectedTile = i; }
        ImGui::PopID();

        if ((i + 1) % tilesPerRow != 0) ImGui::SameLine();
    }

    // ImGui::Text("Selected Tile: %d", selectedTile);
}

void Marmalade::ECS::Tilemap::RenderGrid(Entity* entity) {
    auto* transform = entity->componentManager.GetComponentOfType<Transform>();
    if (!transform) return;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 lineColor = IM_COL32(200, 200, 200, 255);

    glm::vec2 worldOrigin = glm::vec2(transform->pos);

    ImVec2 curPos = ImGui::GetMousePos();
    glm::vec2 mouseWorld = EditorViews::ScreenToWorldSpace(curPos);
    glm::vec2 localPos = mouseWorld - worldOrigin;

    glm::ivec2 hoveredCell = glm::ivec2(
            static_cast<int>(localPos.x / static_cast<float>(tileGrid.cellSize.x)),
            static_cast<int>(localPos.y / static_cast<float>(tileGrid.cellSize.y))
            );

    bool isInside = hoveredCell.x >= 0 && hoveredCell.y >= 0 &&
                    hoveredCell.x < static_cast<int>(tileGrid.size.x) &&
                    hoveredCell.y < static_cast<int>(tileGrid.size.y);

    if (isInside) {
        glm::vec2 cellWorldMin = worldOrigin + glm::vec2(hoveredCell) * glm::vec2(tileGrid.cellSize);
        glm::vec2 cellWorldMax = cellWorldMin + glm::vec2(tileGrid.cellSize);

        ImVec2 screenMin = EditorViews::WorldToScreenSpace(glm::vec3(cellWorldMin, 0.0f));
        ImVec2 screenMax = EditorViews::WorldToScreenSpace(glm::vec3(cellWorldMax, 0.0f));

        drawList->AddRect(screenMin, screenMax, IM_COL32(165, 226, 160, 255), 0.0f, 0, 2.0f);

        ImGui::SetTooltip("tilemap cell: (%d, %d)", hoveredCell.x, hoveredCell.y);

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && isInside) { PaintTile(hoveredCell, selectedTile); }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && isInside) { EraseTile(hoveredCell); }
    }

    for (unsigned int x = 0; x <= tileGrid.size.x; ++x) {
        float worldX = worldOrigin.x + x * static_cast<float>(tileGrid.cellSize.x);
        glm::vec3 worldStart(worldX, worldOrigin.y, 0.0f);
        glm::vec3 worldEnd(worldX, worldOrigin.y + tileGrid.size.y * tileGrid.cellSize.y, 0.0f);

        ImVec2 screenStart = EditorViews::WorldToScreenSpace(worldStart);
        ImVec2 screenEnd = EditorViews::WorldToScreenSpace(worldEnd);

        drawList->AddLine(screenStart, screenEnd, lineColor);
    }

    for (unsigned int y = 0; y <= tileGrid.size.y; ++y) {
        float worldY = worldOrigin.y + y * static_cast<float>(tileGrid.cellSize.y);
        glm::vec3 worldStart(worldOrigin.x, worldY, 0.0f);
        glm::vec3 worldEnd(worldOrigin.x + tileGrid.size.x * tileGrid.cellSize.x, worldY, 0.0f);

        ImVec2 screenStart = EditorViews::WorldToScreenSpace(worldStart);
        ImVec2 screenEnd = EditorViews::WorldToScreenSpace(worldEnd);

        drawList->AddLine(screenStart, screenEnd, lineColor);
    }
}

void Marmalade::ECS::Tilemap::RenderTiles(Entity* entity) const {
    if (!tileSet || !shader) return;

    auto* transform = entity->componentManager.GetComponentOfType<Transform>();
    if (!transform) return;

    shader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform->pos.x, transform->pos.y, 0.0f));
    shader->SetMat4("model", model);

    shader->SetMat4("view", GET_APP.getCamera()->GetView());
    shader->SetMat4("projection", GET_APP.getCamera()->GetProjection());

    shader->SetInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tileSet->texture);

    std::vector<TileInstance> instances;
    instances.reserve(tileGrid.cells.size());

    glm::vec2 worldOrigin = glm::vec2(transform->pos);

    for (int y = 0; y < tileGrid.size.y; ++y) {
        for (int x = 0; x < tileGrid.size.x; ++x) {
            size_t index = y * tileGrid.size.x + x;

            const TileCell& cell = tileGrid.cells[index];
            if (cell.tileIndex < 0) continue;// 0 is an emtpy tile

            const Tile& tile = tileSet->tiles[cell.tileIndex];

            TileInstance inst;
            inst.worldPos = worldOrigin + glm::vec2(x * tileGrid.cellSize.x, y * tileGrid.cellSize.y);
            inst.uvOffset = tile.uvOffset;
            inst.uvSize = tile.uvSize;
            instances.push_back(inst);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(TileInstance), instances.data());

    glBindTexture(GL_TEXTURE_2D, tileSet->texture);
    glBindVertexArray(tileVAO);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instances.size());

    glBindVertexArray(0);
}

void Marmalade::ECS::Tilemap::PaintTile(glm::uvec2 coords, int tileIndex) {
    TileCell* cell = tileGrid.GetCellAt(coords);
    if (!cell) {
        return;
    }

    cell->tileIndex = tileIndex;
}

void Marmalade::ECS::Tilemap::EraseTile(glm::uvec2 coords) {
    TileCell* cell = tileGrid.GetCellAt(coords);
    if (!cell) return;

    cell->tileIndex = -1;
}

void Marmalade::ECS::Tilemap::LoadTextureAtlas() {
    if (!tileSet) tileSet = new Graphics::TileSet();

    std::string path = tileSet->storedConfig.atlas.path;

    unsigned int tex = Texture::LoadTexture(path);
    tileSet->texture = tex;

    glm::uvec2 dim = Texture::GetDimensions(path);
    tileSet->storedConfig.atlas.size = {dim.x, dim.y};

    glm::ivec2 tilesPerRow = {
            (int) (dim.x / tileGrid.cellSize.x),
            (int) (dim.y / tileGrid.cellSize.y)
    };

    tileSet->tiles.clear();
    unsigned int id = 0;

    for (int y = 0; y < tilesPerRow.y; y++) {
        for (int x = 0; x < tilesPerRow.x; x++) {
            Tile t;
            t.id = id++;
            t.uvOffset = glm::vec2(
                    (x * tileGrid.cellSize.x) / static_cast<float>(dim.x),
                    (y * tileGrid.cellSize.y) / static_cast<float>(dim.y)
                    );
            t.uvSize = glm::vec2(
                    tileGrid.cellSize.x / static_cast<float>(dim.x),
                    tileGrid.cellSize.y / static_cast<float>(dim.y)
                    );
            t.texture = tex;
            tileSet->AddTile(t);
        }
    }
}

void Marmalade::ECS::Tilemap::SetupTileQuad() {
    if (tileVAO != 0) return;

    float w = tileGrid.cellSize.x;
    float h = tileGrid.cellSize.y;

    float vertices[]{
        0.0f, 0.0f, 0.0f, 0.0f,
        w,    0.0f, 1.0f, 0.0f,
        w,    h,    1.0f, 1.0f,
        0.0f, h,    0.0f, 1.0f
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &tileVBO);
    glGenBuffers(1, &tileEBO);

    glBindVertexArray(tileVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // Texture UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Marmalade::ECS::Tilemap::SetupTileInstancing() {
    if (instanceVBO != 0) return;

    glGenBuffers(1, &instanceVBO);
    glBindVertexArray(tileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(TileInstance), nullptr, GL_DYNAMIC_DRAW);

    // World Pos
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TileInstance), (void*) 0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // UV Offset
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TileInstance), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // UV Size
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(TileInstance), (void*) (4 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}