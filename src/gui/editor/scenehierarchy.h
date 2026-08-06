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

#ifndef ENGINE_SCENEHIERARCHY_H
#define ENGINE_SCENEHIERARCHY_H

#include "../../scene/entity.h"

class SceneHierarchy {
public:
    void Show();
    void DeselectEntity();
    void SelectEntityByUuid(const std::string& uuid);

private:
    std::weak_ptr<Entity> _parent;
    std::vector<std::weak_ptr<Entity>> _selectedEntities;

    bool _isCreatingEntityChild{false};
    bool _isDeleting{false};
    bool _isRenaming{false};

    void displayEntity(std::shared_ptr<Entity> entity, int index);
    void showCreatePopup();
    void showRenamePopup();
    void showDeletePopup();
    void showContextMenu();
    void createEntity(const std::string& name);

    std::vector<std::shared_ptr<Entity>>& GetEntitiesVectorFor(std::shared_ptr<Entity> entity);
};

#endif
