
#ifndef MARMALADE_PROJECTSETTINGS_H
#define MARMALADE_PROJECTSETTINGS_H

#include <string>
#include <nlohmann/json.hpp>

struct ProjectSettings {
    std::string ProductName{"Marmalade"};
    std::string CompanyName{"Example Company"};
    std::string Description{"A marmalade project"};
    std::string Version{"v0.0.1"};
};

class Settings {
public:
    static ProjectSettings projectSettings;

    static void SaveProjectSettings();
    static void LoadProjectSettings();
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectSettings, ProductName, CompanyName, Description, Version);

#endif//MARMALADE_PROJECTSETTINGS_H
