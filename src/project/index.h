// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
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

#ifndef MARMALADE_PROJECT_INDEX_H
#define MARMALADE_PROJECT_INDEX_H

#include <sqlite3.h>

#include <filesystem>
#include <utility>
#include <vector>

namespace Marmalade::Project {
    class Project;

    class Index {
    public:
        struct Asset {
            std::string uuid;
            std::string path;
        };

        struct Scene {
            std::string uuid;
            std::string name;
            Scene() = default;
            Scene(const std::string& uuid, const std::string& name)
                : uuid(uuid),
                  name(name) {}
        };

        struct Entity {
            std::string uuid;
            std::string name;
            Entity() = default;
            Entity(const std::string& uuid, const std::string& name)
                : uuid(uuid),
                  name(name) {}
        };

        static const char* INFO_TABLE_NAME;
        static const char* ASSETS_TABLE_NAME;
        static const char* SCENES_TABLE_NAME;
        static const char* ENTITIES_TABLE_NAME;

        Index(Project& project, std::filesystem::path file_path)
            : project(project),
              filePath(std::move(file_path)) {}

        void Open();
        void InitTables();
        void BuildAssetsIndex();
        void BuildScenesIndex() const;
        void BuildEntitiesIndex() const;
        void Close();

        void AddAssetsRow(Asset asset) const;
        std::vector<Asset> GetAssetsRows() const;

        void AddScenesRow(Scene scene) const;
        std::vector<Scene> GetScenesRows() const;

        void AddEntitiesRow(Entity entity) const;
        std::vector<Entity> GetEntitiesRows() const;

    private:
        Project& project;
        std::filesystem::path filePath;

        sqlite3* db = nullptr;

        bool tableExists(const char* table_name) const;
        void createTable(const char* table_name, const char* columns) const;

        void upsertInfoRow(const char* key, const char* value) const;
    };
}


#endif
