
#include "projectsettings.h"
#include "application.h"

#include <fstream>

ProjectSettings Settings::projectSettings;

void Settings::SaveProjectSettings() {
    Application& app = Application::GetInstance();
    auto project = app.GetCurrentProject();

    std::ofstream o(std::filesystem::path(project->filePath) / "settings.marm");
    nlohmann::json new_settings = projectSettings;
    o << new_settings.dump(2);
    o.close();
}

void Settings::LoadProjectSettings() {
    Application& app = Application::GetInstance();
    auto project = app.GetCurrentProject();

    std::ifstream i(std::filesystem::path(project->filePath) / "settings.marm");
    if (i.fail()) {
        // File doesn't exist!
        spdlog::error("Failed to load project settings, file does not exist!");
        return;
    }

    auto data = nlohmann::json::parse(i);
    projectSettings = data.template get<ProjectSettings>();
    i.close();
}
