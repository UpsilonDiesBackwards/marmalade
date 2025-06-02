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

#ifndef MARMALADE_GUI_SAVEWORKSPACE_H
#define MARMALADE_GUI_SAVEWORKSPACE_H

#include "../window.h"
#include "../dialog.h"

#include <string>
#include <functional>

namespace Marmalade::GUI {
    class SaveWorkspaceDialog : public CustomDialog {
    public:
        struct CallbackData {
            bool Save;
        };

        std::string workspaceName{};

        void Draw() override;
        std::string GetName() override;

        explicit SaveWorkspaceDialog(std::string workspaceName) : workspaceName(std::move(workspaceName)) {}
    };

    class SaveWorkspaceAsDialog : public CustomDialog {
    public:
        enum DialogType {
            DialogType_SAVE_AS,
            DialogType_DUPLICATE
        };

        void Draw() override;
        std::string GetName() override;

        explicit SaveWorkspaceAsDialog(DialogType type) : _name(""), _type(type) { }

    private:
        DialogType _type;
        char _name[128];
    };
}

#endif
