
#ifndef MARMALADE_GUI_PROJECTWIZARD_H
#define MARMALADE_GUI_PROJECTWIZARD_H

#include <string>
#include <git2/types.h>
#include "../window.h"

struct GitSettings {
    git_repository* repo = NULL;

    bool initGitRepository{false};
    bool useDefaultGitIgnore{true};

    std::string remoteName = "";
    std::string remoteURL = "";
};

namespace Marmalade::GUI {
    class ProjectWizard : public Window {
    public:
        void Draw() override;

        GitSettings gitSettings;
    private:
        std::string projectName = "";
        std::string projectFilePath = "";
        std::string projectFilePathName = "";

        void CreateProject();
        void InitialiseGitRepository(const char* repoPath);
        void SetGitRemoteURL(GitSettings gitSettings);
    };
}

#endif
