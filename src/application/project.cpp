/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "project.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <filesystem>

Project::Project(std::string name, std::string filePath, GitSettings gitSettings, bool loadingProject) : name(name), filePath(filePath), gitSettings(gitSettings) {
    if (loadingProject) { return; }

    CreateProjectDirectories(gitSettings);
}

void Project::CreateProjectDirectories(GitSettings gitSettings) {
    for (const auto& directory : baseDirectories) { // Create Directories
        std::filesystem::path path = std::filesystem::path(filePath) / directory;

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

    for (const auto& file : baseFiles) { // Create Files
        std::filesystem::path path = std::filesystem::path(filePath) / file;

        if (path == std::filesystem::path(filePath) / ".gitignore" && !gitSettings.initGitRepository) { // Create / Skip .gitignore
            spdlog::warn("Project created with Git disabled, skipping '.gitignore' creation");
            continue; // Git has not been enabled, skipping .gitignore
        }

        if (path == std::filesystem::path(filePath) / "README.md" && !gitSettings.createREADME) { // Create / Skip README.md
            spdlog::warn("Project created without README.md");
            continue; // README.md creation has not been enabled, skipping README.md
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
}
