// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
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

#ifndef MARMALADE_GUI_COMPONENTS_FORM
#define MARMALADE_GUI_COMPONENTS_FORM

#include "backgroundlabel.h"

#include <imgui.h>

#include <string>
#include <variant>
#include <vector>

namespace Marmalade::GUI::Components {
    class CustomEntry {
    public:
        std::function<void()> DrawCallback;
    };

    enum class ControlType {
        ControlType_DEFAULT,
        ControlType_SLIDER,
        ControlType_DRAG,
        ControlType_INPUT,
        ControlType_COLOR_EDIT,
        ControlType_COLOR_PICKER,
        ControlType_XYZ,
        ControlType_XY,
        ControlType_Multiline,
    };

    using EntryValuePtr = std::variant<bool*, int*, float*, ImVec4*, std::string*, CustomEntry*>;

    /**
     * Value types and control types:
     *
     * - `bool`        None (only renders as checkbox)
     * - `int`         INPUT, SLIDER, DRAG (default)
     * - `float`       INPUT, SLIDER, DRAG (default)
     * - `ImVec4`      INPUT, SLIDER, DRAG (default), COLOR_EDIT, COLOR_PICKER, XYZ, XY
     * - `std::string` None (default), MULTILINE
     * - `CustomEntry` None
     *
     * Control type parameters:
     * - INPUT:        #SetStep(), #SetFormat()
     * - SLIDER:       #SetRange(), #SetFormat()
     * - DRAG:         #SetRange(), #SetSpeed(), #SetFormat()
     * - XYZ:          #SetMaxWidth(), #SetRange(), #SetSpeed(), #SetFormat()
     * - XY:           #SetMaxWidth(), #SetRange(), #SetSpeed(), #SetFormat()
     */
    class FormEntry {
    public:
        std::string label;
        EntryValuePtr value;

        ControlType control = ControlType::ControlType_DEFAULT;

        float min = 0;
        float max = 0;
        std::string format = "%.3f";
        float step = 0;
        float stepFast = 0;
        float speed = 1;
        float maxWidth = -FLT_MIN;

        std::function<void()> valueChangeCallback = nullptr;

        template<typename T>
        FormEntry(std::string label, T* value) : label(std::move(label)), value(value) {}

        FormEntry& SetControlType(const ControlType controlType) {
            control = controlType;
            return *this;
        }

        FormEntry& SetRange(const float min, const float max) {
            this->min = min;
            this->max = max;
            return *this;
        }

        FormEntry& SetFormat(const std::string& format) {
            this->format = format;
            return *this;
        }

        FormEntry& SetStep(const float step, const float stepFast) {
            this->step = step;
            this->stepFast = stepFast;
            return *this;
        }

        FormEntry& SetSpeed(const float speed) {
            this->speed = speed;
            return *this;
        }

        FormEntry& SetMaxWidth(const float maxWidth) {
            this->maxWidth = maxWidth;
            return *this;
        }

        FormEntry& SetValueChangeCallback(const std::function<void()>& valueChangeCallback) {
            this->valueChangeCallback = valueChangeCallback;
            return *this;
        }
    };

    template<typename T>
    struct ControlRenderer {
        static bool Draw(const std::string& id, T* value, const FormEntry& entry) {
            ImGui::Text("Missing Renderer for type: %s", typeid(T).name());
        }
    };

    template<>
    struct ControlRenderer<bool> {
        static bool Draw(const std::string& id, bool* value, const FormEntry& entry) {
            return ImGui::Checkbox(id.c_str(), value);
        }
    };

    template<>
    struct ControlRenderer<int> {
        static bool Draw(const std::string& id, int* value, const FormEntry& entry) {
            switch (entry.control) {
                case ControlType::ControlType_SLIDER:
                    return ImGui::SliderInt(id.c_str(), value, entry.min, entry.max, entry.format.c_str());
                case ControlType::ControlType_INPUT:
                    return ImGui::InputInt(id.c_str(), value, entry.step, entry.stepFast);
                default:// or ControlType_DRAG
                    return ImGui::DragInt(id.c_str(), value, entry.speed, entry.min, entry.max, entry.format.c_str());
            }
        }
    };

    template<>
    struct ControlRenderer<float> {
        static bool Draw(const std::string& id, float* value, const FormEntry& entry) {
            switch (entry.control) {
                case ControlType::ControlType_SLIDER:
                    return ImGui::SliderFloat(id.c_str(), value, entry.min, entry.max, entry.format.c_str());
                case ControlType::ControlType_INPUT:
                    return ImGui::InputFloat(id.c_str(), value, entry.step, entry.stepFast, entry.format.c_str());
                default:// or ControlType_DRAG
                    return ImGui::DragFloat(id.c_str(), value, entry.speed, entry.min, entry.max, entry.format.c_str());
            }
        }
    };

    template<>
    struct ControlRenderer<ImVec4> {
        static bool Draw(const std::string& id, ImVec4* value, const FormEntry& entry) {
            float availableWidth = ImGui::GetContentRegionAvail().x;

            switch (entry.control) {
                case ControlType::ControlType_SLIDER: {
                    bool valChanged = false;
                    float width = entry.maxWidth < 0 ? availableWidth : std::min(entry.maxWidth, availableWidth);
                    const float widthConstant = 8;

                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::SliderFloat((id + "x").c_str(), &value->x, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::SliderFloat((id + "y").c_str(), &value->y, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::SliderFloat((id + "z").c_str(), &value->z, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::SliderFloat((id + "w").c_str(), &value->w, entry.min, entry.max, entry.format.c_str());
                    return valChanged;
                }
                case ControlType::ControlType_INPUT: {
                    bool valChanged = false;
                    float width = entry.maxWidth < 0 ? availableWidth : std::min(entry.maxWidth, availableWidth);
                    const float widthConstant = 8;

                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::InputFloat((id + "x").c_str(), &value->x, entry.step, entry.stepFast, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::InputFloat((id + "y").c_str(), &value->y, entry.step, entry.stepFast, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::InputFloat((id + "z").c_str(), &value->z, entry.step, entry.stepFast, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::InputFloat((id + "w").c_str(), &value->w, entry.step, entry.stepFast, entry.format.c_str());
                    return valChanged;
                }
                case ControlType::ControlType_COLOR_EDIT: {
                    return ImGui::ColorEdit4(id.c_str(), &value->x);
                }
                case ControlType::ControlType_COLOR_PICKER: {
                    float width = std::min(entry.maxWidth, availableWidth);
                    ImGui::SetNextItemWidth(width);
                    return ImGui::ColorPicker4(id.c_str(), &value->x);
                }
                case ControlType::ControlType_XYZ: {
                    bool valChanged = false;
                    float width = entry.maxWidth < 0 ? availableWidth : std::min(entry.maxWidth, availableWidth);
                    const float widthConstant = 40;

                    BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 3 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "x").c_str(), &value->x, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 3 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "y").c_str(), &value->y, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    BackgroundLabel::DrawInlineLabelWithBackground(" Z ", COL_CATPPUCCIN_UI_BLUE);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 3 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "z").c_str(), &value->z, entry.speed, entry.min, entry.max, entry.format.c_str());
                    return valChanged;
                }
                case ControlType::ControlType_XY: {
                    bool valChanged = false;
                    float width = entry.maxWidth < 0 ? availableWidth : std::min(entry.maxWidth, availableWidth);
                    const float widthConstant = 38;

                    BackgroundLabel::DrawInlineLabelWithBackground(" X ", COL_CATPPUCCIN_UI_RED);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 2 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "x").c_str(), &value->x, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    BackgroundLabel::DrawInlineLabelWithBackground(" Y ", COL_CATPPUCCIN_UI_GREEN);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 2 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "y").c_str(), &value->y, entry.speed, entry.min, entry.max, entry.format.c_str());

                    return valChanged;
                }
                default: {
                    // or ControlType_DRAG
                    bool valChanged = false;
                    float width = entry.maxWidth < 0 ? availableWidth : std::min(entry.maxWidth, availableWidth);
                    const float widthConstant = 8;

                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "x").c_str(), &value->x, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "y").c_str(), &value->y, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "z").c_str(), &value->z, entry.speed, entry.min, entry.max, entry.format.c_str());

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(width / 4 - widthConstant);
                    valChanged |= ImGui::DragFloat((id + "w").c_str(), &value->w, entry.speed, entry.min, entry.max, entry.format.c_str());
                    return valChanged;
                }
            }
        }
    };

    template<>
    struct ControlRenderer<std::string> {
        static int TextInputCallback(ImGuiInputTextCallbackData* data) {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                auto* str = static_cast<std::string*>(data->UserData);
                str->resize(data->BufSize);
                data->Buf = str->data();
            }
            return 0;
        }

        static bool Draw(const std::string& id, std::string* value, const FormEntry& entry) {
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;

            auto buf = value->data();
            const size_t buf_size = value->size() + 1;

            if (entry.control == ControlType::ControlType_Multiline) {
                return ImGui::InputTextMultiline(id.c_str(), buf, buf_size, ImVec2(0, 0), flags, TextInputCallback, value);
            }

            return ImGui::InputText(id.c_str(), buf, buf_size, flags, TextInputCallback, value);
        }
    };

    template<>
    struct ControlRenderer<CustomEntry> {
        static bool Draw(const std::string& id, CustomEntry* value, const FormEntry& entry) {
            if (value && value->DrawCallback) value->DrawCallback();
        }
    };

    class Form {
    public:
        std::vector<FormEntry> entries;

        void AddEntry(FormEntry& entry) {
            entries.push_back(entry);
        }

        void AddEntry(FormEntry&& entry) {
            entries.push_back(entry);
        }

        void DrawForm(bool recalcLabelWidths = false) {
            if (maxLabelWidth == 0 || recalcLabelWidths) {
                maxLabelWidth = 0;
                for (auto& entry: entries) {
                    if (const auto textSize = ImGui::CalcTextSize(entry.label.c_str()); textSize.x > maxLabelWidth) {
                        maxLabelWidth = textSize.x + ImGui::GetStyle().FramePadding.x;
                    }
                }
            }

            if (ImGui::BeginTable("FormTable", 2, ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, maxLabelWidth);
                ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);

                int i = 1;
                for (auto& entry: entries) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();

                    float posX = ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
                                 ImGui::CalcTextSize(entry.label.c_str()).x - ImGui::GetStyle().FramePadding.x;
                    ImGui::SetCursorPosX(posX);
                    ImGui::AlignTextToFramePadding();

                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
                    if (ImGui::Selectable(entry.label.c_str(), false)) {
                        if (std::holds_alternative<bool*>(entry.value)) {
                            bool* val = std::get<bool*>(entry.value);
                            *val = !(*val);
                        }
                        ImGui::SetKeyboardFocusHere(0);
                    }
                    ImGui::PopStyleColor(2);

                    ImGui::TableSetColumnIndex(1);

                    std::string hiddenLabel = std::format("##{}{}", entry.label, i);
                    ImGui::SetNextItemWidth(-FLT_MIN);

                    std::visit([&]<typename T>(T&& val) {
                        using PointerType = std::decay_t<T>;
                        using BaseType = std::remove_pointer_t<PointerType>;

                        if (bool valChanged = ControlRenderer<BaseType>::Draw(hiddenLabel, val, entry); valChanged && entry.valueChangeCallback) {
                            entry.valueChangeCallback();
                        }
                    }, entry.value);
                    i++;
                }

                ImGui::EndTable();
            }
        }

    private:
        float maxLabelWidth = 0.0f;
    };
}

#endif
