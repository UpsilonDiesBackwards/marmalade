
#ifndef MARMALADE_PROJECT_H
#define MARMALADE_PROJECT_H

#include <string>
#include "../../include/scene/scenemanager.h"
#include "../gui/imgui/projectwizard.h"
#include "projectsettings.h"

class Project {
public:
    std::string name;
    std::string filePath; // Where the project is stored / the working directory

    Settings settings;
    GitSettings gitSettings;

    bool loadingProject = false;

    Project(std::string name, std::string filePath, GitSettings gitSettings, bool loadingProject);

private:
    std::vector<std::string> baseDirectories = { // Directories auto-created when the project is made
            "assets",
            "packages",
            "logs"
    };

    std::vector<std::string> baseFiles = { // Files auto-created when the project is made
            "project.marmalade",
            ".gitignore",
            "README.md",
            "settings.marm",
            "package-settings.marm"
    };

    void CreateProjectDirectories(GitSettings gitSettings);
};


#endif//MARMALADE_PROJECT_H
