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

#include "workspaces.h"

#include "../application/application.h"
#include "../application/config/configutil.h"

#include <filesystem>

std::string Marmalade::GUI::WorkspaceManager::targetWorkspacePath;
std::string Marmalade::GUI::WorkspaceManager::currentWorkspacePath;

std::vector<std::string> Marmalade::GUI::WorkspaceManager::_workspaceCache{};

void Marmalade::GUI::WorkspaceManager::LoadWorkspace(const std::filesystem::path& workspacePath) {
    std::string iniPathStr = workspacePath.string();

    targetWorkspacePath = iniPathStr;
    GET_APP.ChangeWorkspace();
}

void Marmalade::GUI::WorkspaceManager::SaveCurrentWorkspace(std::filesystem::path workspacePath) {
    if (workspacePath.empty()) {
        workspacePath = currentWorkspacePath;
    }

    ImGui::SaveIniSettingsToDisk(workspacePath.string().c_str());
}

void Marmalade::GUI::WorkspaceManager::DuplicateWorkspace(std::filesystem::path newWorkspacePath) {
    std::filesystem::copy(currentWorkspacePath, newWorkspacePath);
    currentWorkspacePath = newWorkspacePath.string();
}

void Marmalade::GUI::WorkspaceManager::DeleteCurrentWorkspace(std::filesystem::path nextWorkspace) {
    if (nextWorkspace.empty()) {
        nextWorkspace = GetWorkspacesDir() / "Default.ini";
    }

    std::filesystem::remove(currentWorkspacePath);
    LoadWorkspace(nextWorkspace);
}

std::filesystem::path Marmalade::GUI::WorkspaceManager::GetWorkspacesDir() {
    return Marmalade::ConfigUtil::GetConfigDirectory() / WORKSPACES_DIR_NAME;
}

std::vector<std::string> Marmalade::GUI::WorkspaceManager::GetWorkspaces(bool ignoreCache) {
    if (ignoreCache) _workspaceCache.clear();

    if (!_workspaceCache.empty()) return _workspaceCache;

    for (const auto& entry: std::filesystem::directory_iterator(GetWorkspacesDir())) {
        _workspaceCache.push_back(entry.path().filename().replace_extension("").string());
    }

    return _workspaceCache;
}

std::string Marmalade::GUI::WorkspaceManager::GetCurrentWorkspaceName() {
    return std::filesystem::path(currentWorkspacePath).filename().replace_extension("").string();
}
