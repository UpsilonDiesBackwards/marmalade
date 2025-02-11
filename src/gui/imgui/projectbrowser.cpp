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

#include "projectbrowser.h"

#include "../../application/application.h"

#include <imgui.h>

#include <IconsCodicons.h>

#include <stb/stb_image.h>

#include <spdlog/spdlog.h>

#include <thread>

GLuint Marmalade::GUI::ProjectBrowser::loadTexture(std::string filename) {
    int width, height, channels;
    unsigned char* data = ::stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if (!data) {
        spdlog::error("Failed to load texture: {}", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void Marmalade::GUI::ProjectBrowser::loadTextures() {
    if (_textureOperationRunning.exchange(true)) {
        return;
    }

    glfwMakeContextCurrent(_loadingContext);

    spdlog::info("Loading textures");
    _textureCache.clear();

    for (const auto& item: std::filesystem::directory_iterator(currentPath)) {
        if (item.is_directory()) continue;

        // TODO: Magic number MIME type checking
        GLuint textureId = loadTexture(item.path().string());
        if (!_textureCache.contains(item.path().string())) {
            _textureCache[item.path().string()] = textureId;
        }
    }

    spdlog::info("Textures loaded");
    _texturesLoaded = true;
    _textureOperationRunning = false;

    glfwMakeContextCurrent(nullptr);
}

void Marmalade::GUI::ProjectBrowser::Draw() {
    // Basic context switching
    if (_loadingContext == nullptr) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        _loadingContext = glfwCreateWindow(1, 1, "TextureLoader", nullptr, Application::GetInstance().getWindow());
    }

    ImGui::SetNextWindowPos(ImVec2(256, 128), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(720, 380), ImGuiCond_FirstUseEver);

    ImGui::Begin(ICON_CI_ZOOM_IN " Project Browser", &visible);

    auto* project = Application::GetInstance().GetCurrentProject();

    if (!project) {
        ImGui::Text("Open a project to view its assets");
        ImGui::End();
        return;
    }

    std::filesystem::path rootAssetFolder = std::filesystem::path(project->filePath) / "assets";

    currentPath = rootAssetFolder;

    if (currentPath != rootAssetFolder) {
        if (ImGui::Button("..")) {
            currentPath = currentPath.parent_path();
        }
    }

    if (!_texturesLoaded) {
        std::thread thread(&ProjectBrowser::loadTextures, this);
        thread.detach();
    }

    for (const auto& item: std::filesystem::directory_iterator(currentPath)) {
        const std::filesystem::path path = item.path();
        bool isDirectory = item.is_directory();

        ImGui::BeginGroup();

        bool selected = ImGui::Selectable("##", false, ImGuiSelectableFlags_DontClosePopups);

        if (selected) {
            if (isDirectory) {
                currentPath = path;
            } else {
                // other stuff
            }
        }

        ImTextureID textureId = 0;
        if (_texturesLoaded && _textureCache.contains(path.string())) {
            textureId = _textureCache[path.string()];
        }

        ImGui::SameLine();
        ImGui::Image(textureId, ImVec2(128, 128));
        ImGui::Text("%s", path.filename().string().c_str());

        ImGui::EndGroup();
    }

    ImGui::End();
}
