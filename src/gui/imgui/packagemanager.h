#ifndef MARMALADE_PACKAGEMANAGER_H
#define MARMALADE_PACKAGEMANAGER_H

#include "../window.h"
#include "../../application/config.h"
#include "../components/tableview.h"

#include <string>
#include <mutex>
#include <utility>
#include <atomic>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace Marmalade::GUI {

    enum PackageManagerTab {
        PackageManagerTab_ALL,
        PackageManagerTab_INSTALLED,
        PackageManagerTab_AVAILABLE
    };

    class RepositoriesTableView : public Components::TableView<Repository> {
    public:
        RepositoriesTableView() : TableView<Repository>(Config::engineConfig.Repos, {Components::TableViewColumn("Name"), Components::TableViewColumn("URL")},
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
        struct Package {
            std::string Name{};
            std::string Repo{};
            std::vector<std::string> Authors{};
            std::vector<std::string> Keywords{};

            Package() = default;
            Package(std::string name, std::string repo, std::vector<std::string> authors, const std::vector<std::string>& keywords) : Name(std::move(name)), Repo(std::move(repo)), Authors(std::move(authors)), Keywords(keywords) {}
        };

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

        std::unordered_map<std::string, Package> _packagesByName{};
        std::unordered_map<std::string, std::vector<const Package*>> _keywordIndex{};

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
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Marmalade::GUI::PackageManager::Package, Name, Repo, Authors, Keywords);
}


#endif
