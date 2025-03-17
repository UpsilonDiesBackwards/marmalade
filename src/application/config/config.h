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

#ifndef MARMALADE_CONFIG_H
#define MARMALADE_CONFIG_H

#include "../../gui/dialogs/configerror.h"

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

#include <concepts>
#include <string>
#include <filesystem>
#include <utility>
#include <fstream>
#include <memory>

#define CONFIG_SINGLETON(type)   \
    static type& GetInstance() { \
        static type instance{};  \
        return instance;         \
    }

#define CONFIG_DECL_GET_FUNC(ClassName)                          \
    static inline auto& GetStoredConfig() {                      \
        return Marmalade::ClassName::GetInstance().storedConfig; \
    }

namespace Marmalade {
    /**
     * @brief Concept to check if a given type has a integer `version` member.
     *
     * @tparam TConfig The configuration type to check.
     */
    template<typename TConfig>
    concept HasVersion = requires(TConfig t) {
        { t.version } -> std::convertible_to<int>;
    };

    /**
     * Inherit this class to provide basic config file functionality.
     *
     * @tparam TConfig The configuration type.
     */
    template<typename TConfig>
    class Config : public std::enable_shared_from_this<Config<TConfig>> {
    public:
        std::filesystem::path filePath{};

        bool useGui{false};

        int version{0};
        TConfig storedConfig{};

        explicit Config(std::filesystem::path filePath) : filePath(std::move(filePath)) {
        }

        void AddMigration(int fromVersion, std::function<void(nlohmann::json&)> function) {
            _migrations[fromVersion] = function;
        }

        bool LoadConfig(GUI::ConfigErrorDialog* errorDlg = nullptr) {
            std::ifstream i(filePath);
            if (i.fail()) {
                // File doesn't exist
                storedConfig = {};
                PrepareNewConfig();
                SaveConfig();
                return true;
            }

            nlohmann::json data;
            try {
                data = nlohmann::json::parse(i);
            } catch (const std::exception& ex) {
                handleConfigError(ex, errorDlg);
                return false;
            }

            bool migrated = false;
            int currentVersion = 0;

            if constexpr (HasVersion<TConfig>) {
                currentVersion = data.value("version", 1);
                while (currentVersion < version) {
                    // If the config version is less than the latest, migrate
                    auto it = _migrations.find(currentVersion);
                    if (it != _migrations.end()) {
                        it->second(data);
                        currentVersion++;
                        migrated = true;
                    } else {
                        spdlog::error("Unknown config migration for version {}", currentVersion);
                        break;
                    }
                }
            }

            try {
                storedConfig = data.template get<TConfig>();
            } catch (const std::exception& ex) {
                handleConfigError(ex, errorDlg);
                return false;
            }

            if constexpr (HasVersion<TConfig>) {
                if (migrated) {
                    storedConfig.version = currentVersion;
                    SaveConfig();
                }
            }

            i.close();
            return true;
        }

        void RecreateConfig() {
            PrepareConfigRecreation();

            spdlog::info("Recreate config");
            // Reset config
            storedConfig = TConfig();
            SaveConfig();

            EndConfigRecreation();
        }

        bool SaveConfig() {
            std::ofstream o(filePath);
            nlohmann::json new_config = storedConfig;
            o << new_config.dump(2);
            o.close();
            return true;
        }

        virtual void PrepareNewConfig() {};
        virtual void PrepareConfigRecreation() {};
        virtual void EndConfigRecreation() {};

        TConfig* operator->() {
            return &storedConfig;
        }

    private:
        std::map<int, std::function<void(nlohmann::json&)>> _migrations{};

        void handleConfigError(const std::exception& ex, GUI::ConfigErrorDialog* errorDlg = nullptr) {
            if (useGui && errorDlg != nullptr) {
                // Show error GUI
                errorDlg->fileName = filePath.filename().string();
                errorDlg->errorMsg = ex.what();

                auto self = this->shared_from_this();

                errorDlg->recreateConfigCallback = [self] {
                    self->RecreateConfig();
                };
                errorDlg->visible = true;
            } else {
                spdlog::error("Failed to load config file: {}. Delete the config file or fix the following:", filePath.filename().string());
                spdlog::error(ex.what());
            }
        }
    };
}

#endif
