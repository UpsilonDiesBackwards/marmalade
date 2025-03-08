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
#include "../../application/util.h"
#include "../windowmanager.h"

#include <imgui.h>

#include <IconsCodicons.h>

#include <stb/stb_image.h>

#include <spdlog/spdlog.h>

#include <thread>
#include <fstream>

// Needed for drag and drop
static Marmalade::GUI::ProjectItem projectItem;

void Marmalade::GUI::ProjectBrowser::drawTopBar() {
    const char* modes[] = {ICON_CI_PROJECT " Project", ICON_CI_FILE_DIRECTORY " Files"};

    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::Combo("##Mode", reinterpret_cast<int*>(&_mode), modes, IM_ARRAYSIZE(modes))) {
        // Refresh needed
        _texturesLoaded = "";
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_CI_REFRESH)) {
        _texturesLoaded = "";
    }

    auto* project = Application::GetInstance().GetCurrentProject();
    if (project) {
        ImGui::SameLine();
        ImGui::Text("/%s", _currentPath.generic_string().c_str());
    }

    if (_mode == BrowserMode_PROJECT) {
        ImGui::SameLine();
        ImGui::Text("%s", "Show: ");

        ImGui::SameLine();
        ImGui::Checkbox("Assets", &_showAssets);

        ImGui::SameLine();
        ImGui::Checkbox("Data     ", &_showData);

        ImGui::SameLine();
        ImGui::Checkbox("Source", &_showSrc);
    }

    const float searchBarWidth = 200.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - searchBarWidth);
    ImGui::SetNextItemWidth(searchBarWidth);
    ImGui::InputTextWithHint("##Search", "Filter", _filterText, sizeof(_filterText));
}

void Marmalade::GUI::ProjectBrowser::drawBottomBar() {
    float availableWindowWidth = ImGui::GetContentRegionAvail().x;
    float labelWidth = 7.0f;
    float sliderWidth = (availableWindowWidth - (labelWidth * 0.5) - 18.5f) * 0.3f;

    auto* project = Application::GetInstance().GetCurrentProject();

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
    ImGui::ProgressBar(_texturesLoaded == (project->basePath / _currentPath).string() ? -1.0f : ImGui::GetTime() * -0.2f);

    ImGui::EndChild();
}

void Marmalade::GUI::ProjectBrowser::drawItem(Marmalade::GUI::DirectoryEntry item) {
    const std::filesystem::path path = item.Entry.path();

    GLuint textureId = item.Entry.is_directory() ? _textureCache["directory"] : _textureCache["document"];

    const auto fileType = determineFileType(path.extension());
    if (fileType == FileType_IMAGE) {
        textureId = _textureCache[path.string()];
    }

    ImGui::ImageButton(path.string().c_str(), textureId, ImVec2(_thumbnailSize, _thumbnailSize),
                       ImVec2(0, 1), ImVec2(1, 0));

    if (!item.Entry.is_directory()) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            projectItem = ProjectItem{fileType, path.string()};
            ImGui::SetDragDropPayload("PROJECT_BROWSER_FILE", &projectItem, sizeof(ProjectItem));
            ImGui::Image(textureId, ImVec2(_thumbnailSize, _thumbnailSize));
            ImGui::EndDragDropSource();
        }
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (item.Entry.is_directory()) {
            _texturesLoaded = "";
            _currentPath /= path.filename();
        } else {
            if (item.ClickFunc != nullptr) {
                item.ClickFunc(item);
            }
        }
    }

    std::string label = path.filename().string();
    if (!item.DisplayName.empty()) {
        label = item.DisplayName;
    }

    ImVec2 curPos = ImGui::GetCursorScreenPos();
    ImVec2 textSize = ImGui::CalcTextSize(label.c_str(), nullptr, true);

    ImGui::GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + textSize.x, curPos.y + textSize.y), getBackgroundColor(item.Type));
    ImGui::TextWrapped("%s", label.c_str());
}

void Marmalade::GUI::ProjectBrowser::iterateFiles(std::function<void(DirectoryEntry)> item_callback) {
    auto* project = Application::GetInstance().GetCurrentProject();

    std::vector<DirectoryEntry> items;

    std::unordered_map<CommonDirectory, std::filesystem::path> paths{};
    if (_mode == BrowserMode_PROJECT) {
        paths[CommonDirectory_ASSETS] = _rootAssetDir / _currentPath;
        paths[CommonDirectory_DATA] = _rootDataDir / _currentPath;
        paths[CommonDirectory_SRC] = _rootSrcDir / _currentPath;
    } else {
        paths[CommonDirectory_UNKNOWN] = project->basePath / _currentPath;
    }

    for (const auto& [key, dirPath]: paths) {
        if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
            for (const auto& item: std::filesystem::directory_iterator(dirPath)) {
                items.push_back(DirectoryEntry{item, key});
            }
        }
    }

    std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
        return a.Entry.path().filename().string() < b.Entry.path().filename().string();
    });

    for (const auto& item: items) {
        item_callback(item);
    }
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

void Marmalade::GUI::ProjectBrowser::processItem(Marmalade::GUI::DirectoryEntry& item) {
    std::filesystem::path ext = item.Entry.path().extension();

    if (ext == ".json" || ext == ".marm") {
        // Parse JSON
        std::ifstream i(item.Entry.path());
        if (i.fail()) {
            // File should exist
            return;
        }

        try {
            auto data = nlohmann::json::parse(i);
            if (data["type"] == "Marmalade::Scene") {
                item.DisplayName = data["name"];
                const auto uuid = data["uuid"];
                item.ClickFunc = [uuid](const Marmalade::GUI::DirectoryEntry&) {
                    // Open scene
                    auto& sceneManager = Application::GetInstance().sceneManager;
                    auto* project = Application::GetInstance().GetCurrentProject();

                    Application::GetInstance().editorGUI->sceneHierarchy.DeselectEntity();

                    const std::string fileName = Marmalade::Project::ProjectScenes::GetSceneFileName(uuid);
                    auto newScene = project->scenes.LoadScene(fileName);

                    sceneManager.AddScene(std::make_shared<Scene>(newScene));
                    sceneManager.SetCurrentScene(newScene.GetUuid());
                };
            } else if (data["type"] == "Marmalade::Entity") {
                item.DisplayName = data["name"];
            } else if (data["type"] == "Marmalade::Project::Settings") {
                item.ClickFunc = [](const Marmalade::GUI::DirectoryEntry&) {
                    WindowManager::GetInstance().settings.visible = true;
                };
            }

        } catch (const nlohmann::json::parse_error& e) {
            // Parse error, ignore
        }
    }

    if (item.Type == CommonDirectory_ASSETS) {
        item.ClickFunc = [](const Marmalade::GUI::DirectoryEntry& item) {
            Util::DisplayFile(item.Entry.path().string());
        };
    }
}

ImU32 Marmalade::GUI::ProjectBrowser::getBackgroundColor(Marmalade::GUI::CommonDirectory type) {
    switch (type) {
        case CommonDirectory_ASSETS:
            return Config::engineConfig.projectBrowser.colorAssets;
        case CommonDirectory_DATA:
            return Config::engineConfig.projectBrowser.colorData;
        case CommonDirectory_SRC:
            return Config::engineConfig.projectBrowser.colorSrc;
        default:
            return IM_COL32(0, 0, 0, 0);
    }
}

void Marmalade::GUI::ProjectBrowser::loadTextures() {
    if (_textureOperationRunning.exchange(true)) {
        return;
    }

    glfwMakeContextCurrent(_loadingContext);

    _textureCache.clear();

    _textureCache["directory"] = loadTexture("res/icons/ui/directory.png");
    _textureCache["document"] = loadTexture("res/icons/ui/document.png");

    auto* project = Application::GetInstance().GetCurrentProject();

    iterateFiles([this](const auto& item) {
        if (item.Entry.is_directory()) return;

        if (determineFileType(item.Entry.path().extension()) == FileType_IMAGE) {
            GLuint textureId = loadTexture(item.Entry.path().string());
            if (textureId != 0) {
                _textureCache[item.Entry.path().string()] = textureId;
            }
        }
    });

    _texturesLoaded = (project->basePath / _currentPath).string();
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

    _rootAssetDir = project->basePath / "assets";
    _rootDataDir = project->basePath / "data";
    _rootSrcDir = project->basePath / "src";

    if (!_currentPath.empty()) {
        if (ImGui::Button("..")) {
            _currentPath = _currentPath.parent_path();
            _texturesLoaded = "";
        }
    }

    bool mustLoadFiles = false;
    if (_texturesLoaded != (project->basePath / _currentPath).string()) {
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
        iterateFiles([this](const auto& item) {
            _items.push_back(item);
        });
    }

    for (const auto& item: _items) {
        if ((item.Type == CommonDirectory_ASSETS && _showAssets)//
            || (item.Type == CommonDirectory_DATA && _showData) //
            || (item.Type == CommonDirectory_SRC && _showSrc)   //
            || (item.Type == CommonDirectory_UNKNOWN)) {

            std::string filterText = _filterText;
            if (filterText.empty() || Util::StringToLower(item.Entry.path().filename().string()).find(Util::StringToLower(filterText)) != std::string::npos) {
                DirectoryEntry newItem = item;
                processItem(newItem);

                drawItem(newItem);
                ImGui::NextColumn();
            }
        }
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    // Bottom bar
    drawBottomBar();

    ImGui::End();
}
