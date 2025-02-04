#ifndef MARMALADE_PACKAGEMANAGER_H
#define MARMALADE_PACKAGEMANAGER_H

#include "../window.h"

#include <string>
#include <mutex>
#include <utility>
#include <atomic>
#include <vector>
#include <unordered_map>

namespace Marmalade::GUI {

    enum PackageManagerTab {
        PackageManagerTab_ALL,
        PackageManagerTab_INSTALLED,
        PackageManagerTab_AVAILABLE
    };

    class PackageManager : public Window {
    public:
        struct Package {
            std::string name{};
            std::string author{};
            std::vector<std::string> keywords{};

            Package() = default;
            Package(std::string name, std::string author, const std::vector<std::string>& keywords) : name(std::move(name)), author(std::move(author)), keywords(keywords) {}
        };

        void Draw() override;

        inline void SetProgressText(std::string text) {
            _progressText = std::move(text);
        }

    private:
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
        void cloneRepo();
        void pullRepo();
        void buildIndex();
        void deleteLocalDatabase();
    };
}


#endif
