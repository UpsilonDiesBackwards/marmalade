// Marmalade - Lightweight Game Engine
// Copyright (C) 2025 Tayler Parsons
// Copyright (C) 2025 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_ASSETMETADATA_H
#define MARMALADE_ASSETMETADATA_H

#include <string>
#include <nlohmann/json.hpp>

namespace Marmalade::Project::Assets {
    struct AssetMetadata {
        std::string uuid;
        std::string name;
        std::string filePath;
        std::string type;
        int version;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AssetMetadata, uuid, name, filePath, type, version)
}

#endif //MARMALADE_ASSETMETADATA_H
