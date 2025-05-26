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

#include <fstream>

#include "assetregistry.h"
#include "../application/application.h"
#include "../application/util.h"

std::unordered_map<std::string, Marmalade::Project::Assets::AssetMetadata> Marmalade::Project::Assets::Registry::assets;

const std::map<std::string, std::string> Marmalade::Project::Assets::Registry::extensionType{
        {".mmlmat", "Marmalade::Material"},
        {".animdvr", "Marmalade::Animation::Driver"},
        {".animseq", "Marmalade::Animation::Sequence"},
        {".marm", "Marmalade::Files::Generic::Settings"},
        {".marmalade", "Marmalade::Project::File"},
};

Marmalade::Project::Assets::Registry::Registry()
    : Config(Application::GetInstance().GetCurrentProject()->basePath / ".assetreg") {
}

Marmalade::Project::Assets::Registry& Marmalade::Project::Assets::Registry::GetInstance() {
    static Registry instance;
    return instance;
}

void Marmalade::Project::Assets::Registry::RegisterAsset(const Asset& asset) {
    auto projectRoot = Application::GetInstance().GetCurrentProject()->basePath;
    auto relativePath = std::filesystem::relative(asset.path, projectRoot);

    assets[asset.uuid] = {
            asset.uuid,
            asset.name,
            relativePath.string(),
            asset.type,
            asset.version
    };

    auto path = projectRoot / ".assetreg";
    Save(path);
}

void Marmalade::Project::Assets::Registry::UnregisterAsset(const std::string& uuid) {
    auto it = assets.find(uuid);
    if (it != assets.end()) {
        assets.erase(it);

        auto path = Application::GetInstance().GetCurrentProject()->basePath / ".assetreg";
        Save(path);
    }
}

void Marmalade::Project::Assets::Registry::Save(std::filesystem::path filePath) {
    nlohmann::json j;
    for (const auto& [uuid, meta]: assets) {
        nlohmann::json asset;
        asset["uuid"] = meta.uuid;
        asset["name"] = meta.name;
        asset["filePath"] = meta.filePath;
        asset["type"] = meta.type;
        asset["version"] = meta.version;

        j.push_back(asset);
    }

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

void Marmalade::Project::Assets::Registry::Load(std::filesystem::path filePath) {
    if (!std::filesystem::exists(filePath)) {
        LOG_ERROR("Asset file does not exist: {}", filePath.string().c_str());
        return;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) { return; }

    nlohmann::json j;
    file >> j;

    assets.clear();

    for (const auto& elem: j) {
        AssetMetadata meta;

        meta.uuid = elem.value("uuid", "");
        meta.name = elem.value("name", "");
        meta.filePath = elem.value("filePath", "");
        meta.type = elem.value("type", "");
        meta.version = elem.value("version", 0);

        assets[meta.uuid] = meta;
    }
}

void Marmalade::Project::Assets::Registry::RebuildRegistry() {
    assets.clear();

    std::vector<std::string> assetPaths{"assets"};

    for (std::string directory: assetPaths) {
        for (const auto& entry: std::filesystem::recursive_directory_iterator(
                     Application::GetInstance().GetCurrentProject()->basePath / directory)) {
            if (!entry.is_regular_file()) { continue; }

            std::string ext = entry.path().extension().string();

            auto it = extensionType.find(ext);
            if (it != extensionType.end()) {
                std::ifstream file(entry.path().string());
                if (!file.is_open()) { continue; }

                nlohmann::json assetConfig;
                file >> assetConfig;

                std::string uuid = assetConfig.value("uuid", "");
                std::string name = assetConfig.value("name", entry.path().stem().string());
                std::filesystem::path path = entry.path();
                std::string type = it->second;
                int version = assetConfig.value("version", 1);

                if (uuid.empty()) {
                    LOG_WARN("Skipping asset with missing ID: '{}'", path.string());
                    continue;
                } else {
                    auto existingEntry = assets.find(uuid);
                    if (existingEntry != assets.end()) {
                        existingEntry->second.name = name;
                        existingEntry->second.filePath = std::filesystem::relative(path, Application::GetInstance().GetCurrentProject()->basePath).string();
                        existingEntry->second.version = version;
                        existingEntry->second.type = type;

                        continue;
                    }
                }

                Asset asset(uuid, name, path, type, version);
                RegisterAsset(asset);
            }
        }
    }

    std::unordered_map<std::string, AssetMetadata> cleanedAssets;

    for (const auto& [uuid, meta]: assets) {
        std::filesystem::path fullPath = Application::GetInstance().GetCurrentProject()->basePath / meta.filePath;
        if (std::filesystem::exists(fullPath)) { cleanedAssets[uuid] = meta; } else { LOG_INFO("Removing stale asset: {}", meta.filePath); }
    }

    assets = std::move(cleanedAssets);
}

const Marmalade::Project::Assets::AssetMetadata* Marmalade::Project::Assets::Registry::GetAssetFromUUID(const std::string& uuid) {
    auto it = assets.find(uuid);
    return it != assets.end() ? &it->second : nullptr;
}

const Marmalade::Project::Assets::AssetMetadata* Marmalade::Project::Assets::Registry::GetAssetFromName(const std::string& name) {
    for (const auto& [uuid, meta]: assets) { if (meta.name == name) { return &meta; } }

    return nullptr;
}

void Marmalade::Project::Assets::Registry::Deserialise(const nlohmann::json& json) {
    storedConfig = json.get<AssetRegistryData>();
    assets.clear();

    for (const auto& meta: storedConfig.assets) { assets[meta.uuid] = meta; }
}

void Marmalade::Project::Assets::Registry::PrepareNewConfig() {
    storedConfig.assets.clear();
    for (const auto& [uuid, meta]: assets) { storedConfig.assets.push_back(meta); }

    Config::PrepareNewConfig();
}