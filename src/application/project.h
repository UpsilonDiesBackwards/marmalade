
#ifndef MARMALADE_PROJECT_H
#define MARMALADE_PROJECT_H

#include <string>
#include "../../include/scene/scenemanager.h"

class Project {
public:
    std::string name;
    std::string filePath; // Where the project is stored / the working directory

    Project(std::string name);
};


#endif//MARMALADE_PROJECT_H
