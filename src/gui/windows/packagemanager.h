/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MARMALADE_PACKAGEMANAGER_H
#define MARMALADE_PACKAGEMANAGER_H

#include "../window.h"
#include "../../application/config/engineconfig.h"
#include "../components/tableview.h"
#include "../components/selectablelistview.h"
#include "../../packages/package.h"

#include <string>
#include <mutex>
#include <utility>
#include <atomic>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace Marmalade::GUI {

    class PackageManagerListView : public Components::SelectableListView<Packages::Package> {
    public:
        explicit PackageManagerListView(std::vector<Packages::Package>& items) : SelectableListView(items, {}, {}) {}

        void SetupListView(std::string id, ImVec2 area);

        void RenderItem(const Packages::Package& item, bool selected) override;
    };

    enum PackageManagerTab {
        PackageManagerTab_ALL,
        PackageManagerTab_INSTALLED,
        PackageManagerTab_AVAILABLE
    };

    class RepositoriesTableView : public Components::TableView<Repository> {
    public:
        RepositoriesTableView() : TableView<Repository>(EngineConfig::GetStoredConfig().repos, {Components::TableViewColumn("Name"), Components::TableViewColumn("URL")},
                                                        "PackageManagerRepositoryTable", "Edit Repository", "Remove Repository") {};

        std::vector<std::string> RenderItem(const Repository& item) override;

        void PrepareEdit(const Repository& item) override;
        bool DrawEditDialog(Repository* item) override;
        void ResetEdit() override;

        RemoveDialogResult DrawRemoveDialog(Repository* item) override;

    private:
        char _tempName[256]{0};
        char _tempGitUrl[256]{0};
        int _tempDepth{0};
    };

    class PackageManagerOptions : public Window {
    public:
        void Draw() override;

    private:
        RepositoriesTableView _tableView{};
    };

    class PackageManager : public Window {
    public:
        void Draw() override;

        inline void SetProgressText(std::string text) {
            _progressText = std::move(text);
        }

    private:
        PackageManagerOptions _options;

        std::atomic<bool> _dbOperationRunning{false};

        float _progress{0};
        bool _progressIndeterminate{false};
        std::string _progressText{};

        std::unordered_map<std::string, Packages::Package> _packagesByName{};
        std::unordered_map<std::string, std::vector<const Packages::Package*>> _keywordIndex{};

        std::vector<Packages::Package> _allPackages{};

        // For link confirmation
        std::string _currentUrl{};

        void drawLeftPane(PackageManagerTab tab);
        void drawRightPane(PackageManagerTab tab);
        void drawSplit(PackageManagerTab tab, float bottom_bar_height);
        void drawBottomBar(float height);

        std::string itemId(const char* id, PackageManagerTab tab);

        void handleGitError(const std::string& operation);
        void updateLocalDatabase();
        bool cloneRepo(const Marmalade::Repository& config_repo);
        bool pullRepo(const Marmalade::Repository& config_repo);
        void buildIndex(const Marmalade::Repository& config_repo);
        void deleteLocalDatabase();

        PackageManagerListView _listView{_allPackages};
    };
}


#endif
