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

#ifndef MARMALADE_GUI_COMPONENTS_SELECTABLELISTVIEW
#define MARMALADE_GUI_COMPONENTS_SELECTABLELISTVIEW

#include "util.h"

#include <imgui.h>

#include <type_traits>
#include <utility>

namespace Marmalade::GUI::Components {
    /**
     * @brief A generic list view component for displaying and interacting with sequence containers of items in ImGui.
     *
     * This class provides a UI list view where each item is rendered with a checkbox,
     * allowing selection and interaction via ImGui. It requires items to be selectable,
     * meaning they must implement `SetSelected(bool)` and `IsSelected()`.
     *
     * @tparam T The type of items in the list. Must satisfy `EnableIfSelectable<T>`.
     * @tparam L The container type storing the items. Defaults to `std::vector<T>`.
     * @tparam S_EnableContainer Type trait ensuring `L` is a valid sequence container of `T`.
     * @tparam S_EnableSelectable Type trait ensuring `T` implements selection methods.
     */
    template<typename T, typename L = std::vector<T>,
             typename S_EnableContainer = IsSequenceContainer<T, L>,
             typename S_EnableSelectable = EnableIfSelectable<T>>
    class SelectableListView {
    public:
        /**
         * @brief The container storing the list items.
         */
        L& items;

        /**
         * @brief Constructs a `SelectableListView`.
         *
         * @param items A reference to the container holding the selectable items.
         * @param listViewId A unique identifier for the list view.
         * @param listViewArea The size of the list view in ImGui.
         */
        explicit SelectableListView(L& items, std::string listViewId, ImVec2 listViewArea) : items(items), _listViewId(std::move(listViewId)), _listViewArea(listViewArea) {};

        /**
         * @brief Sets the list view's unique identifier.
         *
         * @param listViewId A new identifier for the list view.
         */
        void SetListViewId(std::string listViewId) {
            _listViewId = std::move(listViewId);
        }

        /**
         * @brief Sets the dimensions of the list view in ImGui.
         *
         * @param listViewArea The new size of the list view.
         */
        void SetListViewArea(ImVec2 listViewArea) {
            _listViewArea = listViewArea;
        }

        T* GetHighlightedItem() {
            if (_highlightedItem == -1 || _highlightedItem > items.size()) {
                return nullptr;
            }

            return &items[_highlightedItem];
        }

        /**
         * @brief Renders the list view UI in ImGui.
         *
         * This function creates an ImGui list box and iterates through `items`,
         * displaying each with a checkbox to toggle selection. It calls `RenderItem()`
         * for each item and updates selection state accordingly.
         */
        void Draw();

        /**
         * @brief Renders a single list item in the UI.
         *
         * This method must be implemented by derived classes to define how individual items are displayed.
         *
         * @param item The item to be rendered.
         * @param selected Whether the item is currently selected.
         */
        virtual void RenderItem(const T& item, bool selected) = 0;

    private:
        std::string _listViewId;///< Unique identifier for the list view.
        ImVec2 _listViewArea;   ///< Size of the list view in ImGui.

        bool _globalDirty{false};///< Tracks if any items have changed selection state.
        int _highlightedItem{-1};///< Stores the item which has been highlighted, not necessarily selected
    };

    /**
     * @brief Draws the selectable list view using ImGui.
     *
     * Iterates through the list, rendering each item with a checkbox to indicate selection.
     * If an item's selection state changes, it updates the `_globalDirty` flag.
     *
     * @tparam T The type of items in the list.
     * @tparam L The container type storing the items.
     * @tparam S_EnableContainer Ensures `L` is a valid sequence container.
     * @tparam S_EnableSelectable Ensures `T` implements selection methods.
     */
    template<typename T, typename L, typename S_EnableContainer, typename S_EnableSelectable>
    void SelectableListView<T, L, S_EnableContainer, S_EnableSelectable>::Draw() {
        if (ImGui::BeginListBox(_listViewId.c_str(), _listViewArea)) {
            int i = 0;
            for (auto& item: items) {
                ImGui::PushID((_listViewId + std::to_string(i)).c_str());

                bool selected = item.IsSelected();
                bool temp = selected;
                bool dirty = false;
                ImVec2 start = ImGui::GetCursorScreenPos();

                if (ImGui::Checkbox("", &temp)) {
                    dirty = true;
                }

                RenderItem(item, selected);

                ImVec2 end = ImVec2(start.x + _listViewArea.x, ImGui::GetCursorScreenPos().y);

                if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(start, end)) {
                    _highlightedItem = i;
                }

                if (_highlightedItem == i) {
                    // TODO: Draw rect behind item
                }

                if (dirty) {
                    item.SetSelected(temp);
                    _globalDirty = true;
                }

                ImGui::Separator();
                ImGui::PopID();
                i++;
            }
            ImGui::EndListBox();
        }
    }
}

#endif
