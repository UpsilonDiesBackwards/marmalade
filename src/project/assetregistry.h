
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

#ifndef MARMALADE_ASSETREGISTRY_H
#define MARMALADE_ASSETREGISTRY_H

#include "assetregistryconfig.h"
#include "assetmetadata.h"

#include "../application/config/config.h"

#include <unordered_map>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace Marmalade::Project::Assets {
    struct Asset;

    class Registry : public Config<AssetRegistryData> {
    public:

        static Registry& GetInstance();
        static std::unordered_map<std::string, AssetMetadata> assets;

        static const std::map<std::string, std::string> extensionType;


        void RegisterAsset(const Asset& asset);
        static void UnregisterAsset(const std::string& uuid);

        static void Save(std::filesystem::path filePath);
        void Load(std::filesystem::path filePath);

        void RebuildRegistry();

        const AssetMetadata* GetAssetFromUUID(const std::string& uuid);
        const AssetMetadata* GetAssetFromName(const std::string& name);

        void Deserialise(const nlohmann::json& json);
        void PrepareNewConfig() override;

    private:
        Registry();
    };

    struct Asset {
        std::string uuid;
        std::string name;
        std::filesystem::path path;
        std::string type;
        int version;

        Asset(const std::string& uuid, const std::string& name, const std::filesystem::path& path, const std::string& type, int version) : uuid(uuid), name(name), path(path), type(type), version(version) {
            Registry::GetInstance().RegisterAsset(*this);
        }

        virtual ~Asset() = default;
    };
}

#endif//MARMALADE_ASSETREGISTRY_H
