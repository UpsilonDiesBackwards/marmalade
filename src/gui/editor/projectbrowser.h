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

#ifndef MARMALADE_PROJECTBROWSER_H
#define MARMALADE_PROJECTBROWSER_H

#include "../window.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <filesystem>
#include <unordered_map>
#include <atomic>
#include <utility>
#include <vector>
#include <functional>

namespace Marmalade::GUI {
    /**
     * \brief The current Browser mode
     * \brief BrowserMode_PROJECT shows project files
     * \brief BrowserMode_FILES shows project data files
     */
    enum BrowserMode {
        BrowserMode_PROJECT,
        BrowserMode_FILES
    };

    /**
     * \brief The display mode of items within the project browser
     * \brief DisplayMode_TILES shows items as tiles with thumbnails
     * \brief DisplayMode_LIST shows items as entries in a list
     */
    enum DisplayMode {
        DisplayMode_TILES,
        DisplayMode_LIST
    };

    /**
     * \brief File type categories
     */
    enum FileType {
        FileType_IMAGE,
        FileType_VIDEO,
        FileType_SCRIPT,
        FileType_CODE,
        FileType_TEXT,
        FileType_UNKNOWN
    };

    static const char* FileTypes[] = {"Image", "Video", "Script", "Code", "Text", "Unknown"};

    /**
     * \brief Common directories of the project structure
     */
    enum CommonDirectory {
        CommonDirectory_ASSETS,
        CommonDirectory_DATA,
        CommonDirectory_SRC,
        CommonDirectory_UNKNOWN
    };

    struct ProjectItem {
        FileType Type;
        std::string Path;
    };

    /**
     * \brief Directory entry of the project browser
     */
    struct DirectoryEntry {
        std::filesystem::directory_entry Entry;
        CommonDirectory Type;
        FileType FileCategory{FileType_UNKNOWN};
        std::string DisplayName;
        std::function<void(DirectoryEntry)> ClickFunc{nullptr};

        DirectoryEntry(std::filesystem::directory_entry entry, CommonDirectory type) : Entry(std::move(entry)), Type(type) {}
    };

    /**
     * \brief The project browser GUI. Will show all the directories and asset items within the currently open project
     */
    class ProjectBrowser : public Window {
    public:
        explicit ProjectBrowser(bool visible) : Window(visible) {};

        void Draw() override;

    private:
        BrowserMode _mode = BrowserMode_PROJECT;
        DisplayMode _displayMode = DisplayMode_TILES;
        float _thumbnailSize = 128.0f;
        float _thumbnailPadding = 16.0f;

        // Only relevant for BrowserMode_PROJECT:
        bool _showAssets = true;
        bool _showData = true;
        bool _showSrc = true;

        // Only relevant for DisplayMode_LIST:
        int _selectedRow = -1;

        char _filterText[512] = "";

        bool _rebuildAssetRegistry = false;

        // Asset creation requests
        bool _createDirectory = false;
        bool _createMaterial = false;

        // Asset default names
        char _newDirectoryName[64] = {};

        // Asset creation paths
        std::string _newDirectoryPath;
        std::string _newMaterialPath;

        std::filesystem::path _rootAssetDir;
        std::filesystem::path _rootDataDir;
        std::filesystem::path _rootSrcDir;

        std::filesystem::path _currentPath;

        GLFWwindow* _loadingContext{nullptr};

        std::vector<DirectoryEntry> _items{};

        std::unordered_map<std::string, ImTextureID> _textureCache{};
        std::atomic<bool> _textureOperationRunning{false};
        std::string _texturesLoaded{};

        void drawTopBar();
        void drawBottomBar();

        /**
         * \brief Draw directory entries as tiles
         * \param item Browser item entry
         */
        void drawItemTile(Marmalade::GUI::DirectoryEntry item);

        /**
         * \brief Draw directory entries as list items
         * \param item Browser item entry
         * \param i Browser item index
         */
        void drawItemList(Marmalade::GUI::DirectoryEntry item, int i);

        /**
         * \brief Iterate all items in the common directories of the project
         * \param item_callback Item
         */
        void iterateFiles(std::function<void(DirectoryEntry)> item_callback);

        /**
         * \brief Determine the file type of an item depending on its extension
         * \param extension Extension of the item
         * \return FileType File type of the item
         */
        FileType determineFileType(const std::filesystem::path& extension);

        /**
         * \brief Process directory items, handles rendering and events for clicking on the item
         * \param item Directory item to process
         */
        void processItem(DirectoryEntry& item);

        /**
         * \brief If item is an image file, get its texture ID
         * \param item Target directory item
         * \return uint Texture ID
         */
        unsigned int getTextureId(const DirectoryEntry& item);

        /**
         * \brief Display context menu for the an item
         * \param item Target directory item
         */
        void displayContextMenu(const DirectoryEntry& item);

        void handleItemDoubleClick(const DirectoryEntry& item);

        /**
         * \brief Creates the ImGui DragDrop Source for the target directory item
         * \param item Target directory item
         * \param textureId TextureID of item to display when dragging
         */
        void handleDrag(const DirectoryEntry& item, const unsigned int textureId);

        /**
         * \brief Display the tooltip for a directory item,
         * \param item Target diretory item
         */
        void displayTooltip(const DirectoryEntry& item);
        ImU32 getBackgroundColor(CommonDirectory type);

        void loadTextures();

        // Asset creation functions
        void CreateDirectory();
        void CreateMaterial();
    };
}

#endif
