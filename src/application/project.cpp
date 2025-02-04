
#include <filesystem>
#include <spdlog/spdlog.h>
#include <fstream>
#include "project.h"
#include "glm/gtx/pca.inl"
Project::Project(std::string name, std::string filePath, GitSettings gitSettings) : name(name), filePath(filePath), gitSettings(gitSettings) {
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
