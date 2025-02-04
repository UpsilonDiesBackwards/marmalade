
#include <filesystem>
#include <spdlog/spdlog.h>
#include <fstream>
#include "project.h"
#include "glm/gtx/pca.inl"
Project::Project(std::string name, std::string filePath, bool createdWithGit) : name(name), filePath(filePath), createdWithGit(createdWithGit) {
    CreateProjectDirectories(createdWithGit);
}

void Project::CreateProjectDirectories(bool git) {
    for (const auto& directory : baseDirectories) { // Create Directories
        std::filesystem::path path = std::filesystem::path(filePath) / directory;

        try {
            if (!std::filesystem::exists(path)) {
                std::filesystem::create_directories(path);
            } else {
                spdlog::error("Error creating directory '{}', already exists!", path.c_str());
            }
        } catch (const std::exception& e) {
            spdlog::error("Error creating directory '{}'", e.what());
        }
    }

    for (const auto& file : baseFiles) { // Create Files
        std::filesystem::path path = std::filesystem::path(filePath) / file;

        if (path == std::filesystem::path(filePath) / ".gitignore" && !git) { // Create / Skip .gitignore
            spdlog::warn("Project created with Git disabled, skipping '.gitignore' creation");
            continue; // Git has not been enabled, skipping .gitignore
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
