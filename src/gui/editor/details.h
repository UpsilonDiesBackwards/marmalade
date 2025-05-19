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

#ifndef MARMALADE_DETAILS_H
#define MARMALADE_DETAILS_H

#include "../window.h"

#include "../dialogs/addcomponent.h"

#include "scene/entity.h"

namespace Marmalade::GUI {
    /**
     * \brief Draws the 'Details' window, this shows the components attached to the selected entity and their values
     */
    class Details : public Window {
    public:
        /**
         * \brief Entity currently selected within scene hierarchy
         */
        Entity* inspectedEntity{nullptr};

        void Draw() override;

        /**
         * \brief If the user is adding a new component to the selected entity, show the 'Add Component' modal dialog window
         * \param adding
         */
        void SetAddingComponent(bool adding) { _addComponentDialog.visible = adding; }

    private:
        Marmalade::ECS::Component* _selectedComponent;

        bool _isRemovingComponent{false};

        AddComponentDialog _addComponentDialog{};

        void ShowRemovePopup();
    };
}

#endif
