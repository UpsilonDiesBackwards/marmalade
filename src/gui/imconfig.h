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

#ifndef MARMALADE_GUI_IMCONFIG_H
#define MARMALADE_GUI_IMCONFIG_H

#include <IconsCodicons.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#if DEBUG

#define IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL 1
#define IMGUI_TEST_ENGINE_ENABLE_IMPLOT 1

#include <imgui_te_imconfig.h>

#define IMGUI_ENABLE_TEST_ENGINE

#endif

#pragma region ImGuiFileDialog

#define USE_STD_FILESYSTEM

#define USE_EXPLORATION_BY_KEYS

#define USE_DIALOG_EXIT_WITH_KEY
#define IGFD_EXIT_KEY ImGuiKey_Escape

#define createDirButtonString ICON_CI_ADD
#define resetButtonString "Reset"
#define devicesButtonString "Devices"
#define editPathButtonString ICON_CI_EDIT
#define dirEntryString ICON_CI_FOLDER
#define linkEntryString ICON_CI_LINK_EXTERNAL
#define fileEntryString ICON_CI_FILE
#define fileNameString "File Name: "
#define dirNameString "Directory Path: "
#define buttonResetSearchString "Reset search"
#define buttonDriveString "Devices"
#define buttonEditPathString "Edit path"
#define buttonResetPathString "Reset to current directory"
#define buttonCreateDirString "Create Directory"
#define OverWriteDialogTitleString "The file already exists! "
#define OverWriteDialogMessageString "Would you like to overwrite it?"
#define OverWriteDialogConfirmButtonString "Confirm"
#define OverWriteDialogCancelButtonString "Cancel"

#define okButtonString "OK"
#define cancelButtonString "Cancel"

#define USE_CUSTOM_SORTING_ICON
#define tableHeaderAscendingIcon ICON_CI_TRIANGLE_UP
#define tableHeaderDescendingIcon ICON_CI_TRIANGLE_DOWN
#define tableHeaderFileNameString " File Name"
#define tableHeaderFileTypeString " Type"
#define tableHeaderFileSizeString " Size"
#define tableHeaderFileDateTimeString " Date"
#define fileSizeBytes "B"
#define fileSizeKiloBytes "KB"
#define fileSizeMegaBytes "MB"
#define fileSizeGigaBytes "GB"


#define USE_PLACES_FEATURE
#define PLACES_PANE_DEFAULT_SHOWN true
#define placesButtonString "Places"
#define placesButtonHelpString "Places"
#define addPlaceButtonString ICON_CI_ADD
#define removePlaceButtonString ICON_CI_REMOVE
#define validatePlaceButtonString ICON_CI_CHECK
#define editPlaceButtonString ICON_CI_EDIT


#define USE_PLACES_BOOKMARKS
// Typo intended
#define PLACES_BOOKMARK_DEFAULT_OPEPEND true
#define placesBookmarksGroupName "Bookmarks"

#define USE_PLACES_DEVICES
#define PLACES_DEVICES_DEFAULT_OPEPEND true
#define placesDevicesGroupName "Devices"

#pragma endregion

#endif
