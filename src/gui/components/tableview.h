#ifndef MARMALADE_GUI_COMPONENTS_TABLEVIEW_H
#define MARMALADE_GUI_COMPONENTS_TABLEVIEW_H

#include <imgui.h>

#include <string>
#include <utility>
#include <type_traits>
#include <vector>

namespace Marmalade::GUI::Components {
    struct TableViewColumn {
        std::string Name;
        ImGuiTableColumnFlags Flags{ImGuiTableColumnFlags_WidthStretch};
        float InitWidthOrWeight{0.0f};

        explicit TableViewColumn(std::string name) : Name(std::move(name)) {};
    };

    template<typename T, typename L>
    using IsSequenceContainer = std::enable_if_t<
            std::is_same_v<typename L::value_type, T> &&
            std::is_same_v<typename L::allocator_type, std::allocator<T>>>;

    template<typename T, typename L = std::vector<T>, typename Enable = IsSequenceContainer<T, L>>
    class TableView {
    public:
        enum RemoveDialogResult {
            RemoveDialogResult_REMOVED,
            RemoveDialogResult_CANCELLED,
            RemoveDialogResult_NONE,
        };

        L& items;

        TableView(L& items, std::vector<TableViewColumn> columns, std::string tableName, std::string editPopupName, std::string removePopupName) : items(items), _columns(std::move(columns)),
                                                                                                                                                   _tableName(std::move(tableName)),
                                                                                                                                                   _editPopupName(std::move(editPopupName)), _removePopupName(std::move(removePopupName)) {};

        void Draw();

        virtual std::vector<std::string> RenderItem(const T& item) = 0;

        virtual void PrepareEdit(const T& item) = 0;
        virtual bool DrawEditDialog(T* item) = 0;
        virtual void ResetEdit() = 0;

        virtual RemoveDialogResult DrawRemoveDialog(T* item) = 0;

    private:
        std::vector<TableViewColumn> _columns;

        std::string _tableName;
        std::string _editPopupName;
        std::string _removePopupName;

        bool _isEditing{false};
        bool _isRemoving{false};
        T* _object{nullptr};
        int _selectedRow{-1};
    };

    template<typename T, typename L, typename Enable>
    void TableView<T, L, Enable>::Draw() {
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
