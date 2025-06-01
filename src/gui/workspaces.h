/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_GUI_WORKSPACES_H
#define MARMALADE_GUI_WORKSPACES_H

#include <filesystem>
#include <vector>

#define WORKSPACES_DIR_NAME "workspaces"

namespace Marmalade::GUI {
    /**
     * \brief Manages Workspaces
     */
    class WorkspaceManager {
    public:
        /**
         * \brief Loads a workspace from disk.
         * \param workspacePath The absolute path to the workspace file.
         */
        static void LoadWorkspace(const std::filesystem::path& workspacePath);

        /**
         * \brief Saves the current workspace to the disk.
         * \param workspacePath The absolute path to the workspace file. Uses the currentWorkspacePath if empty.
         */
        static void SaveCurrentWorkspace(std::filesystem::path workspacePath = "");

        /**
         * \brief Copies a workspace file to a new file. Sets the current workspace to the new path.
         * \param newWorkspacePath The new workspace path.
         */
        static void DuplicateWorkspace(std::filesystem::path newWorkspacePath);

        /**
         * \brief Deletes the current workspace.
         * \param nextWorkspace The workspace to switch to. If empty switches to Default.
         */
        static void DeleteCurrentWorkspace(std::filesystem::path nextWorkspace = "");

        static std::filesystem::path GetWorkspacesDir();

        static std::vector<std::string> GetWorkspaces(bool ignoreCache = false);

        static std::string GetCurrentWorkspaceName();

        /**
         * \brief The target workspace path, used when requesting change of workspace.
         */
        static std::string targetWorkspacePath;

        /**
         * \brief The current workspace path.
         */
        static std::string currentWorkspacePath;

    private:
        static std::vector<std::string> _workspaceCache;
    };
}

#endif
