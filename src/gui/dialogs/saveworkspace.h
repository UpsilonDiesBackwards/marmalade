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

#include <string>
#include <functional>

namespace Marmalade::GUI {
    /**
     * \brief Shows a dialog if a config error has occurred
     */
    class SaveWorkspaceDialog : public Window {
    public:
        std::string workspaceName{};

        std::function<void(bool cancelled, bool save)> callback{nullptr};

        void Draw() override;
    };

    class SaveWorkspaceAsDialog : public Window {
    public:
        enum DialogType {
            DialogType_SAVE_AS,
            DialogType_DUPLICATE
        };

        std::function<void(bool cancelled, std::string name)> callback{nullptr};

        void SetType(DialogType type);
        void Draw() override;

    private:
        DialogType _type;
        char _name[128];
    };
}

#endif
