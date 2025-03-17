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

#include "addcomponent.h"

#include "../../application/application.h"
#include "../../application/util.h"

#include <ecs/component.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

void Marmalade::GUI::AddComponentDialog::Draw() {
    ImGui::SetNextWindowSize(ImVec2(700.0f, 400.0f), ImGuiCond_FirstUseEver);

    WINDOW_BEGIN_MODAL("Add Component", ImGuiWindowFlags_None)

    drawSplit();

    WINDOW_END_MODAL()
}

void Marmalade::GUI::AddComponentDialog::drawRightPane() {
    std::vector<ECS::RegisteredComponent*> components{};
    auto& categoryTree = ECS::ComponentRegistry::Instance().GetCategoryTree();

    if (_selectedTab == "All") {
        auto& allComponents = ECS::ComponentRegistry::Instance().GetRegisteredComponents();
        components = Util::GetValuePointers(allComponents);
    } else if (_selectedTab == "Favourites") {
        components = ECS::ComponentRegistry::Instance().GetFavorites();
    } else {
        if (categoryTree.find(_selectedTab) != categoryTree.end()) {
            components = categoryTree[_selectedTab];
        }
    }

    if (ImGui::BeginListBox("AddComponentRightList", ImGui::GetContentRegionAvail())) {
        for (auto& component: components) {
            ImGui::PushID(("AddComponentRightList" + component->Name).c_str());

            ImGui::Text("%s", component->Name.c_str());
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
                auto* inspectedEntity = Application::GetInstance().editorGUI->details.inspectedEntity;
                inspectedEntity->componentManager.AddComponent(component->Factory->Create(Util::GenerateUUIDv4()));

                visible = false;
                Application::GetInstance().editorGUI->details.SetAddingComponent(false);
            }

            auto& favourites = ECS::ComponentRegistry::Instance().GetFavorites();
            bool isFavourite = Util::Contains(favourites, component);

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20.0f);
            ImGui::Text(isFavourite ? ICON_CI_STAR_FULL : ICON_CI_STAR_EMPTY);

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
                // Toggle favourite
                ECS::ComponentRegistry::Instance().SetFavourite(component->Name, !isFavourite);
                EngineConfig::GetStoredConfig().favouriteComponents.clear();
                for (const auto* comp: favourites) {
                    EngineConfig::GetStoredConfig().favouriteComponents.push_back(comp->Name);
                }

                EngineConfig::GetInstance().SaveConfig();
            }

            ImGui::TextWrapped("%s", component->Description.c_str());

            if (!component->Dependencies.empty()) {
                ImGui::Text("Requires: ");
                for (const auto& dependency: component->Dependencies) {
                    ImGui::SameLine();
                    ImGui::Text("%s", dependency.c_str());
                    ImVec2 min = ImGui::GetItemRectMin();
                    ImVec2 max = ImGui::GetItemRectMax();
                    ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_Border));
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }
}

void Marmalade::GUI::AddComponentDialog::drawLeftPane() {
    if (ImGui::BeginListBox("AddComponentLeftList", ImGui::GetContentRegionAvail())) {
        addRow("Favourites");
        addRow("All");
        ImGui::Separator();

        for (const auto& [category, _]: ECS::ComponentRegistry::Instance().GetCategoryTree()) {
            addRow(category);
        }
        ImGui::EndListBox();
    }
}

void Marmalade::GUI::AddComponentDialog::drawSplit() {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static float split_ratio = 0.3f;
    static float min_size = 50.0f;

    float left_width = area.x * split_ratio;
    float right_width = area.x - left_width - 4.0f;

    left_width = ImMax(left_width, min_size);
    right_width = ImMax(right_width, min_size);

    ImGui::BeginChild("AddComponentLeftPane", ImVec2(left_width, area.y), true);
    drawLeftPane();
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::InvisibleButton("##AddComponentSplitter", ImVec2(8.0f, area.y), ImGuiButtonFlags_None);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    if (ImGui::IsItemActive()) {
        ImGuiIO& io = ImGui::GetIO();
        split_ratio += io.MouseDelta.x / area.x;
        split_ratio = ImClamp(split_ratio, min_size / area.x, 1.0f - min_size / area.x);
    }

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("AddComponentRightPane", ImVec2(right_width, area.y), true);
    drawRightPane();
    ImGui::EndChild();
}

void Marmalade::GUI::AddComponentDialog::addRow(const std::string& category) {
    ImGui::PushID(("AddComponentLeftList" + category).c_str());

    bool selected = (_selectedTab == category);
    if (ImGui::Selectable(category.c_str(), selected)) {
        _selectedTab = category;
    }

    ImGui::PopID();
}
