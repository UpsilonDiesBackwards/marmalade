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

#include "../../../../include/ecs/components/rendering/tilemap.h"

#include "../../../gui/components/backgroundlabel.h"

#include <imgui.h>

#include "../../../application/application.h"
#include "scene/entity.h"

#ifdef _MSC_VER
Marmalade::ECS::TileMap::TileMap() {
    TILEMAP_CTOR_BODY
}
#endif

void Marmalade::ECS::TileMap::Display(Entity* entity) {
    ImGui::Text("%s", name.c_str());

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 65.0f);
        ImGui::TableSetupColumn("Control", ImGuiTableFlags_None);

        // Size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Size");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        bool sizeChanged = false;
        sizeChanged |= ImGui::DragFloat(("##SizeX" + std::to_string(entity->id)).c_str(), &TileGrid.size.x, 0.1f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" Y ", ImVec4(0.65f, 0.75f, 0.50f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        sizeChanged |= ImGui::DragFloat(("##SizeY" + std::to_string(entity->id)).c_str(), &TileGrid.size.y, 0.1f);
        ImGui::PopItemWidth();

        // Spacing
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Spacing");
        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
        Marmalade::GUI::Components::BackgroundLabel::DrawInlineLabelWithBackground(" X ", ImVec4(0.9f, 0.49f, 0.5f, 1.0f));
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        sizeChanged |= ImGui::DragFloat(("##Spacing" + std::to_string(entity->id)).c_str(), &TileGrid.spacing, 0.1f, 0.1f, 256.0f);

        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
}

void Marmalade::ECS::TileMap::Apply(Entity* entity) {
    RenderGUIGrid(entity);
}

void Marmalade::ECS::TileMap::Setup(Entity* entity) {}

void Marmalade::ECS::TileMap::RenderGUIGrid(Entity* entity) const {
    auto transform = entity->componentManager.GetComponentOfType<Transform>();
    float entityPosX = transform->pos.x;
    float entityPosY = transform->pos.y;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 color = IM_COL32(200, 200, 200, 255);

    float spacing = TileGrid.spacing;
    float gridWidth  = TileGrid.size.x;
    float gridHeight = TileGrid.size.y;

    int columns = static_cast<int>(gridWidth  / spacing);
    int rows    = static_cast<int>(gridHeight / spacing);

    float startX = entityPosX - (TileGrid.size.x * 0.5f);
    float startY = entityPosY - (TileGrid.size.y * 0.5f);

    for (int i = 0; i <= columns; ++i) {
        float x = startX + i * spacing;
        if (i == columns) x = startX + gridWidth;
        ImVec2 start = GET_APP.editorGUI->editorViews.WorldToScreenSpace(glm::vec3{x, startY, 0.0f});
        ImVec2 end   = GET_APP.editorGUI->editorViews.WorldToScreenSpace(glm::vec3{x, startY + TileGrid.size.y,0.0f});
        drawList->AddLine(start, end, color);
    }

    for (int j = 0; j <= rows; ++j) {
        float y = startY + j * spacing;
        if (j == rows) y = startY + gridHeight;
        ImVec2 start = GET_APP.editorGUI->editorViews.WorldToScreenSpace(glm::vec3{startX, y,0.0f});
        ImVec2 end   = GET_APP.editorGUI->editorViews.WorldToScreenSpace(glm::vec3{startX + TileGrid.size.x, y, 0.0f});
        drawList->AddLine(start, end, color);
    }
}


nlohmann::json Marmalade::ECS::TileMap::Serialize(const Entity* entity) {
    nlohmann::json j;
    j["size"]["x"] = TileGrid.size.x;
    j["size"]["y"] = TileGrid.size.y;

    j["spacing"]["x"] = TileGrid.spacing;

    return j;
}

void Marmalade::ECS::TileMap::Deserialize(nlohmann::json json, Entity* entity) {
    TileGrid.size.x = json["size"]["x"].get<float>();
    TileGrid.size.y = json["size"]["y"].get<float>();

    TileGrid.spacing = json["spacing"]["x"].get<float>();
}