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

#ifndef MARMALADE_PACKAGES_PACKAGE_H
#define MARMALADE_PACKAGES_PACKAGE_H

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <filesystem>

namespace Marmalade::Packages {
    struct Package {
        std::string Name{};
        std::string Repo{};
        std::vector<std::string> Authors{};
        std::vector<std::string> Keywords{};
        std::filesystem::path LocalPath{};
        bool Selected{false};

        void SetSelected(bool selected) {
            Selected = selected;
        }

        bool IsSelected() {
            return Selected;
        }

        Package() = default;
        Package(std::string name, std::string repo, std::vector<std::string> authors, const std::vector<std::string>& keywords, std::filesystem::path localPath) : Name(std::move(name)), Repo(std::move(repo)), Authors(std::move(authors)), Keywords(keywords), LocalPath(std::move(localPath)) {}
    };

    struct PackageReference {
        std::string repoUrl;
        std::string name;
        std::string version;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Packages::Package, Name, Repo, Authors, Keywords);

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::Packages::PackageReference, repoUrl, name, version);
}


#endif
