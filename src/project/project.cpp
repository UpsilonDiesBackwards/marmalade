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

#include "../application/util.h"

#include <git2/global.h>
#include <git2/repository.h>

#include <fstream>
#include <filesystem>
#include <utility>

Marmalade::Project::Project::Project(const std::filesystem::path& filePath) : filePath(filePath), projectMarmalade(std::make_shared<ProjectFile>(filePath)) {
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
                LOG_ERROR("Error creating directory '{}', already exists!", path.string().c_str());
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error creating directory '{}'", e.what());
        }
    }

    for (const auto& file: baseFiles) {// Create Files
        std::filesystem::path path = basePath / file;

        if (path == basePath / ".gitignore" && !creationOptions.initGitRepository) {// Create / Skip .gitignore
            LOG_WARN("Project created with Git disabled, skipping '.gitignore' creation");
            continue;// Git has not been enabled, skipping .gitignore
        }

        if (path == basePath / "README.md" && !creationOptions.createREADME) {// Create / Skip README.md
            LOG_WARN("Project created without README.md");
            continue;// README.md creation has not been enabled, skipping README.md
        }

        try {
            std::ofstream createdFile(path);
            if (!createdFile) {
                LOG_ERROR("Error creating file: '{}'", file);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error creating file '{}'", e.what());
        }
    }

    // Generate UUID
    projectMarmalade->storedConfig.uuid = Marmalade::Util::GenerateUUIDv4();

    projectMarmalade->SaveConfig();
}

void Marmalade::Project::Project::LoadProjectSettings() {
    std::ifstream i(basePath / projectMarmalade->storedConfig.paths.settings);
    if (i.fail()) {
        // File doesn't exist!
        LOG_ERROR("Failed to load project settings, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    settings = data.template get<ProjectSettings>();
    i.close();
}

void Marmalade::Project::Project::SaveProjectSettings() {
    std::ofstream o(basePath / projectMarmalade->storedConfig.paths.settings);
    nlohmann::json new_settings = settings;
    o << new_settings.dump(2);
    o.close();
}

void Marmalade::Project::Project::LoadProjectPackages() {
    std::ifstream i(basePath / projectMarmalade->storedConfig.paths.packages);
    if (i.fail()) {
        // File doesn't exist!
        LOG_ERROR("Failed to load project packages, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    packages = data.template get<ProjectPackages>();
    i.close();
}

void Marmalade::Project::Project::SaveProjectPackages() {
    std::ofstream o(basePath / projectMarmalade->storedConfig.paths.packages);
    nlohmann::json new_packages = packages;
    o << new_packages.dump(2);
    o.close();
}

bool Marmalade::Project::Project::CheckIfGitRepository() {
    static bool libGit2_init = false;
    if (!libGit2_init) {
        git_libgit2_init();
        libGit2_init = true;
    }

    git_repository *repo = nullptr;

    int err = git_repository_open(&repo, basePath.c_str()); // Attempt to open the git repository at the base path of the project...
    if (err == 0) { //... if no error is returned then the repository exists, return true
        git_repository_free(repo);
        return true;
    }

    git_libgit2_shutdown();
    return false; // No repository exists :'(
}
