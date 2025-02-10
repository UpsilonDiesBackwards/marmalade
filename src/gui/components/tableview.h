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

#ifndef MARMALADE_GUI_COMPONENTS_TABLEVIEW_H
#define MARMALADE_GUI_COMPONENTS_TABLEVIEW_H

#include "util.h"

#include <imgui.h>

#include <string>
#include <utility>
#include <vector>

namespace Marmalade::GUI::Components {
    /**
     * @brief Represents a column in the TableView.
     *
     * Defines column properties such as name, flags, and initial width.
     */
    struct TableViewColumn {
        std::string Name; ///< The name of the column.
        ImGuiTableColumnFlags Flags{ImGuiTableColumnFlags_WidthStretch}; ///< ImGui column flags.
        float InitWidthOrWeight{0.0f}; ///< Initial width or weight of the column.

        /**
         * @brief Constructs a TableViewColumn with a given name.
         * @param name The name of the column.
         */
        explicit TableViewColumn(std::string name) : Name(std::move(name)) {};
    };

    /**
     * @brief A generic, interactive table view for displaying and managing items.
     *
     * This class provides a UI table using ImGui, allowing users to view, edit, and remove items.
     * It requires derived classes to implement item rendering, editing, and removal logic.
     *
     * @tparam T The type of items managed by the table.
     * @tparam L The container type storing the items. Defaults to `std::vector<T>`.
     * @tparam S_Enable Ensures that `L` is a valid sequence container for `T`.
     */
    template<typename T, typename L = std::vector<T>, typename S_Enable = IsSequenceContainer<T, L>>
    class TableView {
    public:
        /**
         * @brief Represents possible results of the remove confirmation dialog.
         */
        enum RemoveDialogResult {
            RemoveDialogResult_REMOVED,   ///< The item was removed.
            RemoveDialogResult_CANCELLED, ///< The removal was canceled.
            RemoveDialogResult_NONE,      ///< No action was taken.
        };

        /**
         * @brief A reference to the container holding the table items.
         */
        L& items;

        /**
         * @brief Constructs a TableView.
         *
         * @param items Reference to the container holding items.
         * @param columns The columns that define the table structure.
         * @param tableName The unique identifier for the table.
         * @param editPopupName The identifier for the edit dialog.
         * @param removePopupName The identifier for the remove confirmation dialog.
         */
        TableView(L& items, std::vector<TableViewColumn> columns, std::string tableName, std::string editPopupName, std::string removePopupName) : items(items), _columns(std::move(columns)),
                                                                                                                                                   _tableName(std::move(tableName)),
                                                                                                                                                   _editPopupName(std::move(editPopupName)), _removePopupName(std::move(removePopupName)) {};

        /**
         * @brief Draws the table view UI.
         *
         * This method renders the table, allowing users to view, edit, and remove items.
         */
        void Draw();

        /**
         * @brief Renders a single row of the table.
         *
         * Must be implemented by derived classes to provide field values for display.
         *
         * @param item The item to render.
         * @return A vector of strings representing table columns.
         */
        virtual std::vector<std::string> RenderItem(const T& item) = 0;

        /**
         * @brief Prepares an item for editing.
         *
         * Called when an item is selected for editing.
         *
         * @param item The item to prepare.
         */
        virtual void PrepareEdit(const T& item) = 0;

        /**
         * @brief Renders the edit dialog.
         *
         * Must be implemented by derived classes to allow item modifications.
         *
         * @param item A pointer to the item being edited.
         * @return True if the edit was completed, false otherwise.
         */
        virtual bool DrawEditDialog(T* item) = 0;

        /**
         * @brief Resets the editing state.
         *
         * Called when an edit dialog is closed.
         */
        virtual void ResetEdit() = 0;

        /**
         * @brief Renders the remove confirmation dialog.
         *
         * Must be implemented by derived classes to confirm item deletion.
         *
         * @param item A pointer to the item being removed.
         * @return The result of the dialog (remove, cancel, or none).
         */
        virtual RemoveDialogResult DrawRemoveDialog(T* item) = 0;

    private:
        std::vector<TableViewColumn> _columns; ///< Columns that define the table structure.

        std::string _tableName;       ///< Unique identifier for the table.
        std::string _editPopupName;   ///< Identifier for the edit popup.
        std::string _removePopupName; ///< Identifier for the remove confirmation popup.

        bool _isEditing{false};  ///< Tracks whether an item is being edited.
        bool _isRemoving{false}; ///< Tracks whether an item is being removed.
        T* _object{nullptr};     ///< Pointer to the item being edited or removed.
        int _selectedRow{-1};    ///< Index of the currently selected row.
    };

    /**
     * @brief Draws the table and handles user interactions.
     *
     * Renders the table, processes item selection, and manages edit/remove dialogs.
     *
     * @tparam T The type of items in the table.
     * @tparam L The container type storing the items.
     * @tparam S_Enable Ensures that `L` is a valid sequence container for `T`.
     */
    template<typename T, typename L, typename S_Enable>
    void TableView<T, L, S_Enable>::Draw() {
        if (ImGui::Button("Add")) {
            _isEditing = true;
            _object = nullptr;
        }

        ImGui::SameLine();
        auto cursor_pos = ImGui::GetCursorPos();
        ImGui::NewLine();

        if (ImGui::BeginTable(_tableName.c_str(), _columns.size(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            for (const auto& col: _columns) {
                ImGui::TableSetupColumn(col.Name.c_str(), col.Flags, col.InitWidthOrWeight);
            }

            ImGui::TableHeadersRow();

            int i = 0;
            for (const auto& item: items) {
                ImGui::TableNextRow();

                bool first{true};
                auto fields = RenderItem(item);
                for (const auto& field: fields) {
                    ImGui::TableNextColumn();

                    if (first) {
                        bool is_selected = (_selectedRow == i);
                        if (ImGui::Selectable(field.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                            _selectedRow = (is_selected ? -1 : i);
                        }
                    } else {
                        ImGui::Text("%s", field.c_str());
                    }

                    first = false;
                }

                i++;
            }

            ImGui::EndTable();
        }

        if (_selectedRow > -1) {
            T& sel_item = items[_selectedRow];

            ImGui::SetCursorPos(cursor_pos);

            if (ImGui::Button("Edit")) {
                _isEditing = true;
                _object = &sel_item;
                PrepareEdit(*_object);
            }
            ImGui::SameLine();

            if (ImGui::Button("Remove")) {
                _isRemoving = true;
                _object = &sel_item;
            }
        }

        if (_isEditing) {
            ImGui::OpenPopup(_editPopupName.c_str());
        }

        if (_isRemoving) {
            ImGui::OpenPopup(_removePopupName.c_str());
        }

        if (ImGui::BeginPopupModal(_editPopupName.c_str())) {
            if (DrawEditDialog(_object)) {
                _isEditing = false;
                ImGui::CloseCurrentPopup();
                ResetEdit();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal(_removePopupName.c_str())) {
            RemoveDialogResult res = DrawRemoveDialog(_object);
            if (res != RemoveDialogResult_NONE) {
                _isRemoving = false;
                ImGui::CloseCurrentPopup();

                if (res == RemoveDialogResult_REMOVED) _selectedRow = -1;
            }

            ImGui::EndPopup();
        }
    }
}

#endif
