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
#include "../../application/plugins/interfaceimpl.h"

#include <ImGuiFileDialog.h>

#include <imgui.h>

#include <IconsCodicons.h>

#include <thread>
#include <fstream>

#ifdef _WIN32

// Causes conflict with CreateDirectory function
#undef CreateDirectory

#endif

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

        _rebuildAssetRegistry = true;
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
    float statusBarHeight = 26.0f;

    auto* project = Application::GetInstance().GetCurrentProject();

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - statusBarHeight);

    ImGui::BeginChild("BottomBar", ImVec2(0, statusBarHeight), false);

    // View buttons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (ImGui::Button(ICON_CI_LAYOUT)) _displayMode = DisplayMode_TILES;
    ImGui::SetItemTooltip("Show as Tiles");
    ImGui::SameLine();
    ImGui::PopStyleVar();
    if (ImGui::Button(ICON_CI_LIST_FLAT)) _displayMode = DisplayMode_LIST;
    ImGui::SetItemTooltip("Show as List");
    ImGui::SameLine();

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

void Marmalade::GUI::ProjectBrowser::drawItemTile(Marmalade::GUI::DirectoryEntry item) {
    const std::filesystem::path path = item.Entry.path();

    unsigned int textureId = getTextureId(item);

    ImGui::ImageButton(path.string().c_str(), textureId, ImVec2(_thumbnailSize, _thumbnailSize),
                       ImVec2(0, 1), ImVec2(1, 0));

    displayContextMenu(item);
    handleDrag(item, textureId);
    handleItemDoubleClick(item);
    displayTooltip(item);

    std::string label = path.filename().string();
    if (!item.DisplayName.empty()) {
        label = item.DisplayName;
    }

    ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

    const float labelBoxBkgWidth = _thumbnailSize + 10.0f;
    const float labelBoxBkgHeight = textSize.y + 8.0f;
    const float labelBoxBkgRounding = 6.0f;

    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImVec2 rectMin = cur;
    ImVec2 rectMax = ImVec2(cur.x + labelBoxBkgWidth, cur.y + labelBoxBkgHeight);

    ImU32 bgCol = IM_COL32(49, 50, 68, 170);
    ImU32 borderCol = getBackgroundColor(item.Type);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, bgCol, labelBoxBkgRounding);
    drawList->AddRect(rectMin, rectMax, borderCol, labelBoxBkgRounding, 0, 1.8f);

    float textY = rectMin.y + (labelBoxBkgHeight - textSize.y) * 0.5f;

    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 4.0f, textY));
    ImGui::TextWrapped("%s", label.c_str());

    ImGui::SetCursorScreenPos(ImVec2(rectMin.x, rectMax.y + 4.0f));
    ImGui::Dummy(ImVec2(0,0));
}

void Marmalade::GUI::ProjectBrowser::drawItemList(Marmalade::GUI::DirectoryEntry item, int i) {
    ImGui::PushID(item.Entry.path().string().c_str());

    std::string label = item.Entry.path().filename().string();
    if (!item.DisplayName.empty()) {
        label = item.DisplayName;
    }

    unsigned int textureId = getTextureId(item);

    ImGui::TableNextColumn();
    bool isSelected = (_selectedRow == i);
    if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
        _selectedRow = (isSelected ? -1 : i);
    }

    displayContextMenu(item);
    handleDrag(item, textureId);
    handleItemDoubleClick(item);
    displayTooltip(item);

    ImGui::TableNextColumn();
    ImGui::Text("%s", FileTypes[item.FileCategory]);

    ImGui::PopID();
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

void Marmalade::GUI::ProjectBrowser::processItem(Marmalade::GUI::DirectoryEntry& item) {
    std::filesystem::path ext = item.Entry.path().extension();

    item.FileCategory = determineFileType(ext);

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

unsigned int Marmalade::GUI::ProjectBrowser::getTextureId(const Marmalade::GUI::DirectoryEntry& item) {
    GLuint textureId = item.Entry.is_directory() ? _textureCache["directory"] : _textureCache["document"];
    if (item.FileCategory == FileType_IMAGE) {
        textureId = _textureCache[item.Entry.path().string()];
    }

    return textureId;
}

void Marmalade::GUI::ProjectBrowser::displayContextMenu(const Marmalade::GUI::DirectoryEntry& item) {
    std::string contextMenuId = "ProjectBrowserItemContextMenu:";
    contextMenuId += item.Entry.path().string();
    if (ImGui::BeginPopupContextWindow(contextMenuId.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::BeginMenu("Create New")) {
            if (ImGui::MenuItem("Directory")) {
                _createDirectory = true;
                _newMaterialPath = item.Entry.path().string();
                std::memset(_newDirectoryName, 0, sizeof(_newDirectoryName));
            }

            if (ImGui::MenuItem("Material")) {
                _createMaterial = true;
            }

            if (ImGui::MenuItem("Lua Script")) {
            }

            if (ImGui::MenuItem("Text Document")) {
            }

            if (ImGui::MenuItem("Animation Driver")) {
            }

            if (ImGui::MenuItem("Animation")) {
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Delete")) {
            std::filesystem::remove(item.Entry.path());
            _texturesLoaded = "";

            for (const auto& [uuid, meta] : Marmalade::Project::Assets::Registry::assets) {
                if (meta.filePath == item.Entry.path()) {
                    Marmalade::Project::Assets::Registry::UnregisterAsset(uuid);
                    break;
                }
            }

            _rebuildAssetRegistry = true;
        }

        ImGui::EndPopup();
    }
}

void Marmalade::GUI::ProjectBrowser::handleItemDoubleClick(const Marmalade::GUI::DirectoryEntry& item) {
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (item.Entry.is_directory()) {
            _texturesLoaded = "";
            _currentPath /= item.Entry.path().filename();
        } else {
            if (item.ClickFunc != nullptr) {
                item.ClickFunc(item);
            }
        }
    }
}

void Marmalade::GUI::ProjectBrowser::handleDrag(const Marmalade::GUI::DirectoryEntry& item, unsigned int textureId) {
    if (!item.Entry.is_directory()) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            projectItem = ProjectItem{item.FileCategory, item.Entry.path().string()};
            ImGui::SetDragDropPayload("PROJECT_BROWSER_FILE", &projectItem, sizeof(ProjectItem));
            ImGui::Image(textureId, ImVec2(_thumbnailSize, _thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndDragDropSource();
        }
    }
}

void Marmalade::GUI::ProjectBrowser::displayTooltip(const Marmalade::GUI::DirectoryEntry& item) {
    if (ImGui::BeginItemTooltip()) {
        if (!item.DisplayName.empty()) {
            ImGui::Text("%s", item.DisplayName.c_str());
        }
        ImGui::Text("%s", item.Entry.path().filename().string().c_str());

        ImGui::EndTooltip();
    }
}

ImU32 Marmalade::GUI::ProjectBrowser::getBackgroundColor(Marmalade::GUI::CommonDirectory type) {
    switch (type) {
        case CommonDirectory_ASSETS:
            return EngineConfig::GetStoredConfig().projectBrowser.colorAssets;
        case CommonDirectory_DATA:
            return EngineConfig::GetStoredConfig().projectBrowser.colorData;
        case CommonDirectory_SRC:
            return EngineConfig::GetStoredConfig().projectBrowser.colorSrc;
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

    _textureCache["directory"] = Util::LoadGuiTexture("res/icons/ui/directory.png");
    _textureCache["document"] = Util::LoadGuiTexture("res/icons/ui/document.png");

    auto* project = Application::GetInstance().GetCurrentProject();

    iterateFiles([this](const auto& item) {
        if (item.Entry.is_directory()) return;

        if (determineFileType(item.Entry.path().extension()) == FileType_IMAGE) {
            GLuint textureId = Util::LoadGuiTexture(item.Entry.path().string());
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

    WINDOW_BEGIN(ICON_CI_ZOOM_IN " Project Browser", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)

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

    ImGui::BeginChild("AssetListTiles", ImVec2(0, -22), true);

    if (mustLoadFiles) {
        _items.clear();
        iterateFiles([this](const auto& item) {
            _items.push_back(item);
        });
    }

    bool continueDraw = true;

    if (_displayMode == DisplayMode_TILES) {
        ImGui::Columns(columnCount, nullptr, false);
    } else if (_displayMode == DisplayMode_LIST) {
        std::vector<GUI::Components::TableViewColumn> columns = {Components::TableViewColumn("Name"), Components::TableViewColumn("Type")};

        continueDraw = ImGui::BeginTable("AssetListList", columns.size(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY);

        if (continueDraw) {
            ImGui::TableSetupScrollFreeze(0, 1);
            for (const auto& col: columns) {
                ImGui::TableSetupColumn(col.Name.c_str(), col.Flags, col.InitWidthOrWeight);
            }

            ImGui::TableHeadersRow();
        }
    }

    int i = 0;
    if (continueDraw) {
        for (const auto& item: _items) {
            if ((item.Type == CommonDirectory_ASSETS && _showAssets)//
                || (item.Type == CommonDirectory_DATA && _showData) //
                || (item.Type == CommonDirectory_SRC && _showSrc)   //
                || (item.Type == CommonDirectory_UNKNOWN)) {

                std::string filterText = _filterText;
                if (filterText.empty() || Util::StringToLower(item.Entry.path().filename().string()).find(Util::StringToLower(filterText)) != std::string::npos) {
                    DirectoryEntry newItem = item;
                    processItem(newItem);

                    if (_displayMode == DisplayMode_TILES) {
                        drawItemTile(newItem);
                        ImGui::NextColumn();
                    } else if (_displayMode == DisplayMode_LIST) {
                        ImGui::TableNextRow();

                        drawItemList(newItem, i);

                        i++;
                    }
                }
            }
        }
    }

    if (_displayMode == DisplayMode_TILES) {
        ImGui::Columns(1);
    } else if (_displayMode == DisplayMode_LIST) {
        if (continueDraw) ImGui::EndTable();
    }
    ImGui::EndChild();

    // Bottom bar
    drawBottomBar();

    if (_rebuildAssetRegistry) {
        Marmalade::Project::Assets::Registry::GetInstance().RebuildRegistry();
        _rebuildAssetRegistry = false;
    }

    CreateDirectory();
    CreateMaterial();

    WINDOW_END()
}

void Marmalade::GUI::ProjectBrowser::CreateDirectory() {
    if (_createDirectory) {
        ImGui::OpenPopup("Create New Directory");
        _createDirectory = false;
    }

    if (ImGui::BeginPopupModal("Create New Directory", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Directory Name", _newDirectoryName, IM_ARRAYSIZE(_newDirectoryName));

        if (ImGui::Button("Create") && std::strlen(_newDirectoryName) > 0) {
            std::filesystem::path newDirPath = _rootAssetDir / _newDirectoryName; // TODO: Make this the currently open directory
            if (!std::filesystem::exists(newDirPath)) {
                std::filesystem::create_directory(newDirPath);
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Marmalade::GUI::ProjectBrowser::CreateMaterial() {
    if (_createMaterial) {
        IGFD::FileDialogConfig config;
        config.path = GET_APP.GetCurrentProject()->basePath.string();
        config.flags = ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("CreateMaterial", "New Material", ".mmlmat", config);
        _createMaterial = false;
    }

    if (ImGuiFileDialog::Instance()->Display("CreateMaterial")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            _newMaterialPath = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string selectedFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

            std::string uuid = Util::GenerateUUIDv4();
            Marmalade::Material::Material newMaterial(_newMaterialPath, selectedFileName, uuid);

            newMaterial.SaveConfig();
        }
        ImGuiFileDialog::Instance()->Close();
    }
}
