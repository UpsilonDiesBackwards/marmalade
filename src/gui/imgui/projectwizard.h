/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_GUI_PROJECTWIZARD_H
#define MARMALADE_GUI_PROJECTWIZARD_H

#include "../window.h"

#include <git2/types.h>

#include <string>

struct GitSettings {
    git_repository* repo = NULL;

    bool initGitRepository{false};
    bool useDefaultGitIgnore{true};
    bool createREADME{false};

    std::string remoteName = "";
    std::string remoteURL = "";
    std::string readmeText = "";
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
