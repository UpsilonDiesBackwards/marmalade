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
    enum BrowserMode {
        BrowserMode_PROJECT,
        BrowserMode_FILES
    };

    enum DisplayMode {
        DisplayMode_TILES,
        DisplayMode_LIST
    };

    enum FileType {
        FileType_IMAGE,
        FileType_VIDEO,
        FileType_SCRIPT,
        FileType_CODE,
        FileType_TEXT,
        FileType_UNKNOWN
    };

    static const char* FileTypes[] = {"Image", "Video", "Script", "Code", "Text", "Unknown"};

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

    struct DirectoryEntry {
        std::filesystem::directory_entry Entry;
        CommonDirectory Type;
        FileType FileCategory{FileType_UNKNOWN};
        std::string DisplayName;
        std::function<void(DirectoryEntry)> ClickFunc{nullptr};

        DirectoryEntry(std::filesystem::directory_entry entry, CommonDirectory type) : Entry(std::move(entry)), Type(type) {}
    };

    class ProjectBrowser : public Window {
    public:
        explicit ProjectBrowser(bool visible) : Window(visible) {};

        void Draw() override;

    private:
        BrowserMode _mode = BrowserMode_PROJECT;
        DisplayMode _displayMode = DisplayMode_TILES;
        float _thumbnailSize = 128.0f;
        float _thumbnailPadding = 8.0f;

        // Only relevant for BrowserMode_PROJECT:
        bool _showAssets = true;
        bool _showData = true;
        bool _showSrc = true;

        // Only relevant for DisplayMode_LIST:
        int _selectedRow = -1;

        char _filterText[512] = "";

        // Asset creation requests
        bool _createMaterial = false;

        // Asset creation paths
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
        void drawItemTile(Marmalade::GUI::DirectoryEntry item);
        void drawItemList(Marmalade::GUI::DirectoryEntry item, int i);

        void showRMBContextMenu();

        void iterateFiles(std::function<void(DirectoryEntry)> item_callback);
        FileType determineFileType(const std::filesystem::path& extension);
        void processItem(DirectoryEntry& item);

        unsigned int getTextureId(const DirectoryEntry& item);
        void displayContextMenu(const DirectoryEntry& item);
        void handleItemDoubleClick(const DirectoryEntry& item);
        void handleDrag(const DirectoryEntry& item, const unsigned int textureId);
        void displayTooltip(const DirectoryEntry& item);
        ImU32 getBackgroundColor(CommonDirectory type);

        void loadTextures();

        // Asset creation functions
        void CreateMaterial();
    };
}

#endif
