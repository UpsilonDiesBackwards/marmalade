
#ifndef MARMALADE_PROJECT_H
#define MARMALADE_PROJECT_H

#include <string>
#include "../../include/scene/scenemanager.h"

class Project {
public:
    std::string name;
    std::string filePath; // Where the project is stored / the working directory

    bool createdWithGit = false;

    Project(std::string name, std::string filePath, bool createdWithGit);

private:
    std::vector<std::string> baseDirectories = { // Directories auto-created when the project is made
            "assets",
            "packages",
    };

    std::vector<std::string> baseFiles = { // Files auto-created when the project is made
            "project.marmalade",
            ".gitignore",
            "settings.marm",
            "package-settings.marm"
    };

    void CreateProjectDirectories(bool git);
};


#endif//MARMALADE_PROJECT_H
