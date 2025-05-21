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

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "editor/editor.h"

#include "editor/topbar.h"
#include "editor/scenehierarchy.h"
#include "editor/details.h"
#include "dialogs/autosave.h"

class Editor {
public:
    EditorViews editorViews;
    SceneHierarchy sceneHierarchy;

    Marmalade::GUI::TopBar topBar;
    Marmalade::GUI::Details details;

    Marmalade::GUI::AutoSave autoSaveUI;

    // If `true` then ui element will be opened on startup
    bool showEditorViews = true;
    bool showSceneHeirarchy = true;
    bool showDetails = true;

    void Render();
};


#endif
