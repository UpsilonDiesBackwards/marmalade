
#ifndef MARMALADE_GUI_PROJECTWIZARD_H
#define MARMALADE_GUI_PROJECTWIZARD_H

#include <string>
#include "../window.h"

namespace Marmalade::GUI {
    class ProjectWizard : public Window {
    public:
        void Draw() override;

    private:
        std::string projectName = "";
        std::string projectFilePath = "";
        std::string projectFilePathName = "";

        bool initGitRepository{false};

        void CreateProject();
        void InitialiseGitRepository(const char* repoPath);
    };
}

#endif
