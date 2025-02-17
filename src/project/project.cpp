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

#include "project.h"

#include "../application/application.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <filesystem>
#include <utility>

Marmalade::Project::Project::Project(const std::filesystem::path& filePath) : filePath(filePath) {
    if (!filePath.filename().string().ends_with("project.marmalade")) {
        throw std::runtime_error("File path must be path to project.marmalade");
    }

    basePath = filePath.parent_path();
}

Marmalade::Project::Project::Project(std::string name, const std::filesystem::path& filePath) : Project(filePath) {
    this->name = std::move(name);
}

void Marmalade::Project::Project::CreateEmptyProject(ProjectCreationOptions creationOptions) {
    for (const auto& directory: baseDirectories) {// Create Directories
        std::filesystem::path path = basePath / directory;

        try {
            if (!std::filesystem::exists(path)) {
                std::filesystem::create_directories(path);
            } else {
                spdlog::error("Error creating directory '{}', already exists!", path.string().c_str());
            }
        } catch (const std::exception& e) {
            spdlog::error("Error creating directory '{}'", e.what());
        }
    }

    for (const auto& file: baseFiles) {// Create Files
        std::filesystem::path path = basePath / file;

        if (path == basePath / ".gitignore" && !creationOptions.initGitRepository) {// Create / Skip .gitignore
            spdlog::warn("Project created with Git disabled, skipping '.gitignore' creation");
            continue;// Git has not been enabled, skipping .gitignore
        }

        if (path == basePath / "README.md" && !creationOptions.createREADME) {// Create / Skip README.md
            spdlog::warn("Project created without README.md");
            continue;// README.md creation has not been enabled, skipping README.md
        }

        try {
            std::ofstream createdFile(path);
            if (!createdFile) {
                spdlog::error("Error creating file: '{}'", file);
            }
        } catch (const std::exception& e) {
            spdlog::error("Error creating file '{}'", e.what());
        }
    }

    SaveProjectMarmalade();
}

void Marmalade::Project::Project::LoadProjectMarmalade() {
    std::ifstream i(filePath);
    if (i.fail()) {
        // File doesn't exist!
        spdlog::error("Failed to load project marmalade, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    projectMarmalade = data.template get<ProjectMarmalade>();
    i.close();
}

void Marmalade::Project::Project::SaveProjectMarmalade() {
    std::ofstream o(filePath);
    nlohmann::json new_settings = projectMarmalade;
    o << new_settings.dump(2);
    o.close();
}

void Marmalade::Project::Project::SaveProjectSettings() {
    std::ofstream o(basePath / projectMarmalade.paths.settings);
    nlohmann::json new_settings = settings;
    o << new_settings.dump(2);
    o.close();
}

void Marmalade::Project::Project::LoadProjectSettings() {
    std::ifstream i(basePath / projectMarmalade.paths.settings);
    if (i.fail()) {
        // File doesn't exist!
        spdlog::error("Failed to load project settings, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    settings = data.template get<ProjectSettings>();
    i.close();
}
