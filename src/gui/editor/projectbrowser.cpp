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

#include "imgui.h"

#include "IconsCodicons.h"

#include "stb/stb_image.h"

#include "spdlog/spdlog.h"

#include <thread>

// Needed for drag and drop
static Marmalade::GUI::ProjectItem projectItem;

void Marmalade::GUI::ProjectBrowser::drawTopBar() {
    if (ImGui::Button(ICON_CI_REFRESH)) {
        _texturesLoaded = "";
    }

    ImGui::SameLine();
    ImGui::Text("%s", _currentPath.string().c_str());
}

void Marmalade::GUI::ProjectBrowser::drawBottomBar() {
    float availableWindowWidth = ImGui::GetContentRegionAvail().x;
    float labelWidth = 7.0f;
    float sliderWidth = (availableWindowWidth - (labelWidth * 0.5) - 18.5f) * 0.3f;

    float statusBarHeight = 26.0f;

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - statusBarHeight);

    ImGui::BeginChild("BottomBar", ImVec2(0, statusBarHeight), false);

    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderFloat("Size", &_thumbnailSize, 16, 512);
    ImGui::SameLine();
    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderFloat("Padding", &_thumbnailPadding, 0, 128);

    ImGui::SameLine();
    ImGui::PushItemWidth(sliderWidth);
    ImGui::ProgressBar(_texturesLoaded == _currentPath.string() ? -1.0f : ImGui::GetTime() * -0.2f);

    ImGui::EndChild();
}

Marmalade::GUI::FileType Marmalade::GUI::ProjectBrowser::determineFileType(const std::filesystem::path& extension) {
    if (extension == ".png" || extension == ".jpg" || extension == ".gif") {
        return FileType_IMAGE;
    } else if (extension == ".mp4" || extension == ".avi") {
        return FileType_VIDEO;
    } else if (extension == ".lua") {
        return FileType_SCRIPT;
    } else if (extension == ".cpp") {
        return FileType_CODE;
    } else if (extension == ".txt") {
        return FileType_TEXT;
    }
    return FileType_UNKNOWN;
}

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

    _textureCache.clear();

    _textureCache["directory"] = loadTexture("res/icons/ui/directory.png");
    _textureCache["document"] = loadTexture("res/icons/ui/document.png");

    for (const auto& item: std::filesystem::directory_iterator(_currentPath)) {
        if (item.is_directory()) continue;

        if (determineFileType(item.path().extension()) == FileType_IMAGE) {
            GLuint textureId = loadTexture(item.path().string());
            if (textureId != 0) {
                _textureCache[item.path().string()] = textureId;
            }
        }
    }

    _texturesLoaded = _currentPath.string();
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

    // Top bar
    drawTopBar();

    auto* project = Application::GetInstance().GetCurrentProject();

    if (!project) {
        ImGui::Text("Open a project to view its assets");
        ImGui::End();
        return;
    }

    _rootAssetDir = std::filesystem::path(project->basePath) / "assets";
    if (_currentPath.empty()) {
        _currentPath = _rootAssetDir;
    }

    if (_currentPath != _rootAssetDir) {
        if (ImGui::Button("..")) {
            _currentPath = _currentPath.parent_path();
        }
    }

    bool mustLoadFiles = false;
    if (_texturesLoaded != _currentPath.string()) {
        mustLoadFiles = true;
        std::thread thread(&ProjectBrowser::loadTextures, this);
        thread.detach();
    }

    float cellSize = _thumbnailSize + _thumbnailPadding;

    float panelWidth = ImGui::GetContentRegionAvail().x;

    int columnCount = std::max(1, int(panelWidth / cellSize));

    ImGui::BeginChild("AssetList", ImVec2(0, -22), true);

    ImGui::Columns(columnCount, nullptr, false);

    if (mustLoadFiles) {
        _items.clear();
        for (const auto& item: std::filesystem::directory_iterator(_currentPath)) {
            _items.push_back(item);
        }
    }

    for (const auto& item: _items) {
        const std::filesystem::path path = item.path();

        GLuint textureId = item.is_directory() ? _textureCache["directory"] : _textureCache["document"];

        const auto fileType = determineFileType(path.extension());
        if (fileType == FileType_IMAGE) {
            textureId = _textureCache[path.string()];
        }

        ImGui::ImageButton(path.string().c_str(), textureId, ImVec2(_thumbnailSize, _thumbnailSize),
                           ImVec2(0, 1), ImVec2(1, 0));

        if (!item.is_directory()) {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                projectItem = ProjectItem{fileType, path.string()};
                ImGui::SetDragDropPayload("PROJECT_BROWSER_FILE", &projectItem, sizeof(ProjectItem));
                ImGui::Image(textureId, ImVec2(_thumbnailSize, _thumbnailSize));
                ImGui::EndDragDropSource();
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (item.is_directory()) {
                _currentPath /= path.filename();
            }
        }

        ImGui::TextWrapped("%s", path.filename().string().c_str());

        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    // Bottom bar
    drawBottomBar();

    ImGui::End();
}
