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

const char* Marmalade::Project::Project::MARM_DIR = ".marm";

Marmalade::Project::Project::Project(const std::filesystem::path& filePath) : filePath(filePath),
                                                                              projectMarmalade(std::make_shared<ProjectFile>(filePath)) {
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

    projectSettings = std::make_shared<SettingsContainer>(
            SettingsObject("assets", Config<ProjectAssets>("")),
            SettingsObject("packages", Config<ProjectPackages>("")),
            SettingsObject("settings", Config<ProjectSettings>("")),
            SettingsObject("user", Config<ProjectUser>("")));

    initSettingsObject(projectSettings->assets);
    initSettingsObject(projectSettings->packages);
    initSettingsObject(projectSettings->settings);
    initSettingsObject(projectSettings->user);

    std::ofstream marmGitignore(basePath / MARM_DIR / ".gitignore");
    marmGitignore << "user.marm" << std::endl
                  << "index.marmdb" << std::endl;
    marmGitignore.close();

    std::ofstream gitignore(basePath / ".gitignore");
    gitignore << "packages/" << std::endl;
    gitignore.close();

    if (creationOptions.createREADME) {
        std::ofstream readme(basePath / "README.md");
        readme << creationOptions.readmeText << std::endl;
        readme.close();
    }

    if (creationOptions.initGitRepository) {
        // TODO: Initialise git repo with libgit
    }

    // Generate UUID
    projectMarmalade->storedConfig.uuid = Util::GenerateUUIDv4();

    projectMarmalade->SaveConfig();
}

void Marmalade::Project::Project::LoadProjectSettings(GUI::ConfigErrorDialog* errorDlg) {
    projectSettings = std::make_shared<SettingsContainer>(
            SettingsObject("assets", Config<ProjectAssets>("")),
            SettingsObject("packages", Config<ProjectPackages>("")),
            SettingsObject("settings", Config<ProjectSettings>("")),
            SettingsObject("user", Config<ProjectUser>("")));

    loadSettingsObject(projectSettings->assets, errorDlg);
    loadSettingsObject(projectSettings->packages, errorDlg);
    loadSettingsObject(projectSettings->settings, errorDlg);
    loadSettingsObject(projectSettings->user, errorDlg);
}

void Marmalade::Project::Project::SaveProjectSettings() const {
    projectSettings->assets.config.SaveConfig();
    projectSettings->packages.config.SaveConfig();
    projectSettings->settings.config.SaveConfig();
    projectSettings->user.config.SaveConfig();
}

void Marmalade::Project::Project::LoadProjectPackages() {
    // TODO: Initialise project packages
}

void Marmalade::Project::Project::InitDatabase() {
    projectIndex = std::make_shared<Index>(*this, basePath / projectMarmalade->GetPathOrDefault("index", std::string(MARM_DIR) + "/" + "index.marmdb"));
    projectIndex->Open();
}

void Marmalade::Project::Project::CloseDatabase() {
    projectIndex->Close();
}

bool Marmalade::Project::Project::CheckIfGitRepository() const {
    static bool libGit2_init = false;
    if (!libGit2_init) {
        git_libgit2_init();
        libGit2_init = true;
    }

    git_repository* repo = nullptr;

    int err = git_repository_open(&repo, basePath.string().c_str());// Attempt to open the git repository at the base path of the project...
    if (err == 0) {                                                 //... if no error is returned then the repository exists, return true
        git_repository_free(repo);
        return true;
    }

    git_libgit2_shutdown();
    return false;// No repository exists :'(
}
