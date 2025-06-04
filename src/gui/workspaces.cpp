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

#include <GLFW/glfw3.h>

#include <zip.h>

#include <filesystem>

std::string Marmalade::GUI::WorkspaceManager::targetWorkspacePath;
std::string Marmalade::GUI::WorkspaceManager::currentWorkspacePath;

std::vector<std::string> Marmalade::GUI::WorkspaceManager::_workspaceCache{};

void Marmalade::GUI::WorkspaceManager::LoadWorkspace(const std::filesystem::path& workspacePath) {
    std::string iniPathStr = workspacePath.string();

    // Find topology
    auto targetTopo = GetCurrentTopology();
    auto topos = GetTopologiesForWorkspace(iniPathStr);
    auto it = topos.find(targetTopo);
    if (it != topos.end()) {
        iniPathStr = it->second;
    }

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

void Marmalade::GUI::WorkspaceManager::ExportCurrentWorkspace(std::filesystem::path exportPath) {
    struct zip_t* zip = zip_open(exportPath.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

    auto topologies = GetTopologiesForWorkspace(currentWorkspacePath);
    for (const auto& [topo, file]: topologies) {
        zip_entry_open(zip, std::filesystem::path(file).filename().string().c_str());
        zip_entry_fwrite(zip, file.c_str());
        zip_entry_close(zip);
    }

    zip_close(zip);
}

void Marmalade::GUI::WorkspaceManager::ImportWorkspace(std::filesystem::path importPath) {
    struct zip_t* zip = zip_open(importPath.string().c_str(), 0, 'r');

    int n = zip_entries_total(zip);
    for (int i = 0; i < n; i++) {
        zip_entry_openbyindex(zip, i);
        const char* name = zip_entry_name(zip);
        zip_entry_fread(zip, (GetWorkspacesDir() / name).string().c_str());
        zip_entry_close(zip);
    }

    zip_close(zip);
}

std::filesystem::path Marmalade::GUI::WorkspaceManager::GetWorkspacesDir() {
    return Marmalade::ConfigUtil::GetConfigDirectory() / WORKSPACES_DIR_NAME;
}

std::vector<std::string> Marmalade::GUI::WorkspaceManager::GetWorkspaces(bool ignoreCache) {
    if (ignoreCache) _workspaceCache.clear();

    if (!_workspaceCache.empty()) return _workspaceCache;

    for (const auto& entry: std::filesystem::directory_iterator(GetWorkspacesDir())) {
        if (!entry.is_regular_file() || entry.path().extension() != ".ini") {
            continue;
        }

        auto filename = entry.path().filename().replace_extension("").string();

        size_t atPos = filename.find('@');
        std::string workspaceName = (atPos != std::string::npos) ? filename.substr(0, atPos) : filename;

        if (std::find(_workspaceCache.begin(), _workspaceCache.end(), workspaceName) == _workspaceCache.end()) {
            _workspaceCache.push_back(workspaceName);
        }
    }

    return _workspaceCache;
}

std::unordered_map<std::string, std::string> Marmalade::GUI::WorkspaceManager::GetTopologiesForWorkspace(std::filesystem::path workspace) {
    std::unordered_map<std::string, std::string> topologies{};

    auto targetWorkspace = workspace.filename().replace_extension("").string();

    for (const auto& entry: std::filesystem::directory_iterator(GetWorkspacesDir())) {
        if (!entry.is_regular_file() || entry.path().extension() != ".ini") {
            continue;
        }

        auto filename = entry.path().filename().replace_extension("").string();

        size_t atPos = filename.find('@');
        if (atPos == std::string::npos) {
            if (filename == targetWorkspace) {
                topologies["*"] = entry.path().string();
            }
            continue;
        }

        std::string workspaceName = filename.substr(0, atPos);
        if (workspaceName != targetWorkspace) continue;

        std::string suffix = filename.substr(atPos + 1);
        topologies[suffix] = entry.path().string();
    }

    return topologies;
}

std::string Marmalade::GUI::WorkspaceManager::GetCurrentWorkspaceName() {
    auto filename = std::filesystem::path(currentWorkspacePath).filename().replace_extension("").string();

    size_t atPos = filename.find('@');
    if (atPos == std::string::npos) {
        return filename;
    }

    return filename.substr(0, atPos);
}

bool Marmalade::GUI::WorkspaceManager::IsTopologyWorkspace() {
    size_t atPos = currentWorkspacePath.find('@');
    return atPos != std::string::npos;
}

std::string Marmalade::GUI::WorkspaceManager::GetCurrentTopology() {
    std::stringstream topo;

    int count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    for (int i = 0; i < count; i++) {
        int width, height = 0;
        glfwGetMonitorWorkarea(monitors[i], nullptr, nullptr, &width, &height);
        if (width > height) {
            topo << "h";
        } else {
            topo << "v";
        }
    }

    return topo.str();
}

void Marmalade::GUI::WorkspaceManager::MonitorConfigCallback(GLFWmonitor* monitor, int event) {
    if (EngineConfig::GetStoredConfig().appearance.automaticWorkspaceSwitching) {
        LoadWorkspace(GetWorkspacesDir() / (GetCurrentWorkspaceName() + ".ini"));
    }
}
