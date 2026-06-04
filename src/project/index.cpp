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

#include "index.h"

#include "project.h"
#include "../application/logger.h"

const char* Marmalade::Project::Index::INFO_TABLE_NAME = "_info";
const char* Marmalade::Project::Index::ASSETS_TABLE_NAME = "assets";
const char* Marmalade::Project::Index::SCENES_TABLE_NAME = "scenes";
const char* Marmalade::Project::Index::ENTITIES_TABLE_NAME = "entities";

void Marmalade::Project::Index::Open() {
    if (int res = sqlite3_open(filePath.string().c_str(), &db); res != SQLITE_OK) {
        LOG_ERROR("Failed to open SQLite database {}: {}", filePath.string(), res);
        throw std::runtime_error("Failed to open project database.");
    }

    InitTables();
}

void Marmalade::Project::Index::InitTables() {
    createTable(INFO_TABLE_NAME, "key TEXT NOT NULl, value TEXT NOT NULL, PRIMARY KEY(key)");
    upsertInfoRow("version", "1");

    if (!tableExists(ASSETS_TABLE_NAME)) {
        createTable(ASSETS_TABLE_NAME, "uuid TEXT NOT NULL, path TEXT NOT NULL, PRIMARY KEY(uuid)");
        BuildAssetsIndex();
    }

    if (!tableExists(SCENES_TABLE_NAME)) {
        createTable(SCENES_TABLE_NAME, "uuid TEXT NOT NULL, name TEXT NOT NULL, PRIMARY KEY(uuid)");
        BuildScenesIndex();
    }

    if (!tableExists(ENTITIES_TABLE_NAME)) {
        createTable(ENTITIES_TABLE_NAME, "uuid TEXT NOT NULL, name TEXT NOT NULL, PRIMARY KEY(uuid)");
        BuildEntitiesIndex();
    }
}

void Marmalade::Project::Index::BuildAssetsIndex() {
    LOG_INFO("Building assets index");
    // TODO: Read assets.marm file, and add to index here
}

void Marmalade::Project::Index::BuildScenesIndex() const {
    LOG_INFO("Building scenes index");

    for (const auto& item: std::filesystem::directory_iterator(project.basePath / "data")) {
        try {
            if (std::ifstream file(item.path()); file.is_open()) {
                nlohmann::json j;
                file >> j;
                file.close();

                if (j["type"] == "Marmalade::Scene") {
                    AddScenesRow(Scene(j["uuid"], j["name"]));
                }
            }
        } catch (const std::exception& ex) {
            LOG_ERROR("{}", ex.what());
        }
    }
}

void Marmalade::Project::Index::BuildEntitiesIndex() const {
    LOG_INFO("Building entities index");
    for (const auto& item: std::filesystem::directory_iterator(project.basePath / "data" / "entities")) {
        try {
            if (std::ifstream file(item.path()); file.is_open()) {
                nlohmann::json j;
                file >> j;
                file.close();

                if (j["type"] == "Marmalade::Entity") {
                    AddEntitiesRow(Entity(j["uuid"], j["name"]));
                }
            }
        } catch (const std::exception& ex) {
            LOG_ERROR("{}", ex.what());
        }
    }
}

void Marmalade::Project::Index::Close() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void Marmalade::Project::Index::AddAssetsRow(Asset asset) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "INSERT INTO " << ASSETS_TABLE_NAME << "(uuid, path) VALUES (?, ?);";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to upsert row to assets table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    sqlite3_bind_text(stmt, 1, asset.uuid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, asset.path.c_str(), -1, SQLITE_STATIC);

    if (int res = sqlite3_step(stmt); res != SQLITE_DONE) {
        LOG_ERROR("Failed to upsert row to assets table: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }

    sqlite3_finalize(stmt);
}

std::vector<Marmalade::Project::Index::Asset> Marmalade::Project::Index::GetAssetsRows() const {
    std::vector<Asset> results;
    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "SELECT uuid, path FROM " << ASSETS_TABLE_NAME << ";";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to get rows from assets table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Asset row;

        const unsigned char* uuidPtr = sqlite3_column_text(stmt, 0);
        const unsigned char* pathPtr = sqlite3_column_text(stmt, 1);

        if (uuidPtr) row.uuid = reinterpret_cast<const char*>(uuidPtr);
        if (pathPtr) row.path = reinterpret_cast<const char*>(pathPtr);

        results.push_back(row);
    }

    sqlite3_finalize(stmt);

    return results;
}

void Marmalade::Project::Index::AddScenesRow(Scene scene) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "INSERT INTO " << SCENES_TABLE_NAME << "(uuid, name) VALUES (?, ?);";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to upsert row to scenes table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    sqlite3_bind_text(stmt, 1, scene.uuid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, scene.name.c_str(), -1, SQLITE_STATIC);

    if (int res = sqlite3_step(stmt); res != SQLITE_DONE) {
        LOG_ERROR("Failed to upsert row to scenes table: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }

    sqlite3_finalize(stmt);
}

std::vector<Marmalade::Project::Index::Scene> Marmalade::Project::Index::GetScenesRows() const {
    std::vector<Scene> results;
    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "SELECT uuid, name FROM " << SCENES_TABLE_NAME << ";";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to get rows from scenes table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Scene row;

        const unsigned char* uuidPtr = sqlite3_column_text(stmt, 0);
        const unsigned char* namePtr = sqlite3_column_text(stmt, 1);

        if (uuidPtr) row.uuid = reinterpret_cast<const char*>(uuidPtr);
        if (namePtr) row.name = reinterpret_cast<const char*>(namePtr);

        results.push_back(row);
    }

    sqlite3_finalize(stmt);

    return results;
}

void Marmalade::Project::Index::AddEntitiesRow(Entity entity) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "INSERT INTO " << ENTITIES_TABLE_NAME << "(uuid, name) VALUES (?, ?);";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to upsert row to entities table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    sqlite3_bind_text(stmt, 1, entity.uuid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, entity.name.c_str(), -1, SQLITE_STATIC);

    if (int res = sqlite3_step(stmt); res != SQLITE_DONE) {
        LOG_ERROR("Failed to upsert row to entities table: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }

    sqlite3_finalize(stmt);
}

std::vector<Marmalade::Project::Index::Entity> Marmalade::Project::Index::GetEntitiesRows() const {
    std::vector<Entity> results;
    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "SELECT uuid, name FROM " << ENTITIES_TABLE_NAME << ";";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to get rows from entities table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Entity row;

        const unsigned char* uuidPtr = sqlite3_column_text(stmt, 0);
        const unsigned char* namePtr = sqlite3_column_text(stmt, 1);

        if (uuidPtr) row.uuid = reinterpret_cast<const char*>(uuidPtr);
        if (namePtr) row.name = reinterpret_cast<const char*>(namePtr);

        results.push_back(row);
    }

    sqlite3_finalize(stmt);

    return results;
}

bool Marmalade::Project::Index::tableExists(const char* table_name) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    sqlite3_stmt* stmt;
    auto sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    bool exists = false;

    if (int res = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to execute SQL: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }

    sqlite3_bind_text(stmt, 1, table_name, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = true;
    }

    sqlite3_finalize(stmt);
    return exists;
}

void Marmalade::Project::Index::createTable(const char* table_name, const char* columns) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    std::stringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << table_name << "(" << columns << ");";

    if (int res = sqlite3_exec(db, sql.str().c_str(), nullptr, nullptr, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to execute SQL: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }
}

void Marmalade::Project::Index::upsertInfoRow(const char* key, const char* value) const {
    if (db == nullptr) throw std::runtime_error("db is null");

    sqlite3_stmt* stmt;
    std::stringstream sql;
    sql << "INSERT INTO " << INFO_TABLE_NAME << "(key, value) VALUES (?, ?) ON CONFLICT(key) DO UPDATE SET value=excluded.value;";

    if (int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr); res != SQLITE_OK) {
        LOG_ERROR("Failed to prepare to upsert row to info table: {}", res);
        throw std::runtime_error("Failed to prepare SQL");
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC);

    if (int res = sqlite3_step(stmt); res != SQLITE_DONE) {
        LOG_ERROR("Failed to upsert row to info table: {}", res);
        throw std::runtime_error("Failed to execute SQL");
    }

    sqlite3_finalize(stmt);
}
