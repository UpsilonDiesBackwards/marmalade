
#ifndef MARMALADE_PROJECT_H
#define MARMALADE_PROJECT_H

#include <string>
#include "../../include/scene/scenemanager.h"
#include "../gui/imgui/projectwizard.h"

class Project {
public:
    std::string name;
    std::string filePath; // Where the project is stored / the working directory

    GitSettings gitSettings;

    Project(std::string name, std::string filePath, GitSettings gitSettings);

private:
    std::vector<std::string> baseDirectories = { // Directories auto-created when the project is made
            "assets",
            "packages",
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
