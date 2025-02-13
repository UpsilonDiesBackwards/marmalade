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

namespace Marmalade::GUI {
    enum FileType {
        FileType_IMAGE,
        FileType_VIDEO,
        FileType_SCRIPT,
        FileType_CODE,
        FileType_TEXT,
        FileType_UNKNOWN
    };

    class ProjectBrowser : public Window {
    public:
        explicit ProjectBrowser(bool visible) : Window(visible) {};

        void Draw() override;

    private:
        std::filesystem::path _currentPath;
        std::filesystem::path _rootAssetDir;

        GLFWwindow* _loadingContext{nullptr};

        std::unordered_map<std::string, ImTextureID> _textureCache{};
        std::atomic<bool> _textureOperationRunning{false};
        std::string _texturesLoaded{};

        FileType determineFileType(const std::filesystem::path& extension);

        GLuint loadTexture(std::string filename);
        void loadTextures();
    };
}

#endif
