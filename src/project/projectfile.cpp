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

#include "projectfile.h"

Marmalade::Project::ProjectFile::ProjectFile(const std::filesystem::path& filePath) : Config<ProjectMarmalade>(filePath) {
    version = PROJECT_FILE_VERSION;
    useGui = true;
}

const std::string& Marmalade::Project::ProjectFile::GetPathOrDefault(const std::string& key, const std::string& def) {
    if (!storedConfig.paths.contains(key)) {
        storedConfig.paths[key] = def;
        return def;
    }

    return storedConfig.paths[key];
}
