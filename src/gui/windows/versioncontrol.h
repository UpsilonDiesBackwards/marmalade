
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

#ifndef MARMALADE_VERSIONCONTROL_H
#define MARMALADE_VERSIONCONTROL_H

#include "../window.h"

#include <git2/repository.h>

#include <vector>
#include <string>

namespace Marmalade::GUI {
    class VersionControl : public Window {
    public:
        git_repository *repo = nullptr;
        git_status_list * statusList = nullptr;

        void Draw() override;
    private:
        std::string _selectedFile;
        std::vector<std::string> _modifiedFiles;

        std::vector<std::string> oldContent;
        std::vector<std::string> newContent;

        bool _showDiffViewer = false;

        void CheckForModifiedItems();
        void DisplayModifiedItems();

        std::vector<std::string> GetNewVersionLines(const std::string &file_path);
        std::vector<std::string> GetOldVersionLines(const std::string &file_path);

        void GetItemDiff(const std::string file);
        void ShowItemDiff(const std::vector<std::string> &oldContent, const std::vector<std::string> &newContent);
        void SaveMergedFile(const std::string& path, const std::vector<std::string>& lines);
    };
}

#endif//MARMALADE_VERSIONCONTROL_H
