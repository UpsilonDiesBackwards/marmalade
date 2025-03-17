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

#include "packagemanager.h"

#include "../../application/util.h"
#include "../components/markdownparser.h"
#include "../fontmanager.h"
#include "../../application/config/configutil.h"

#include <sstream>
#include <thread>
#include <filesystem>
#include <fstream>
#include <utility>

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <git2.h>

#include <spdlog/spdlog.h>

#define LOCAL_REPO_PATH "local-repo"
#define INDEX_FILENAME "index.json"

using namespace Marmalade::GUI;

#pragma region List view

void PackageManagerListView::SetupListView(std::string id, ImVec2 area) {
    SetListViewId(std::move(id));
    SetListViewArea(area);
}

void PackageManagerListView::RenderItem(const Packages::Package& item, bool selected) {
    ImGui::SameLine();
    ImGui::Text("%s - %s", item.Name.c_str(), item.Repo.c_str());

    std::ostringstream authors;
    std::copy(item.Authors.begin(), item.Authors.end() - 1, std::ostream_iterator<std::string>(authors, ", "));
    authors << item.Authors.back();

    ImGui::Text("    %s", authors.str().c_str());
}

#pragma endregion

#pragma region Table view

std::vector<std::string> RepositoriesTableView::RenderItem(const Marmalade::Repository& item) {
    return {item.name, item.gitUrl};
}

void RepositoriesTableView::PrepareEdit(const Marmalade::Repository& item) {
    std::strncpy(_tempName, item.name.c_str(), sizeof(_tempName) - 1);
    std::strncpy(_tempGitUrl, item.gitUrl.c_str(), sizeof(_tempGitUrl) - 1);
    _tempDepth = item.depth;
}

bool RepositoriesTableView::DrawEditDialog(Marmalade::Repository* item) {
    ImGui::InputText("Name", _tempName, 256);
    ImGui::InputText("Git URL", _tempGitUrl, 256);
    ImGui::InputInt("depth", &_tempDepth);

    if (ImGui::Button("Cancel")) {
        return true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Save")) {
        if (item == nullptr) {
            // Create
            items.push_back(Repository{_tempName, _tempGitUrl, _tempDepth});
        } else {
            item->name = _tempName;
            item->gitUrl = _tempGitUrl;
            item->depth = _tempDepth;
        }

        EngineConfig::GetInstance().SaveConfig();

        return true;
    }

    return false;
}

void RepositoriesTableView::ResetEdit() {
    memset(_tempName, 0, 256);
    memset(_tempGitUrl, 0, 256);
    _tempDepth = 0;
}

Components::TableView<Marmalade::Repository, std::vector<Marmalade::Repository>, void>::RemoveDialogResult RepositoriesTableView::DrawRemoveDialog(Marmalade::Repository* item) {
    ImGui::Text("Are you sure you want to delete %s", item->name.c_str());

    if (ImGui::Button("Yes")) {
        items.erase(std::remove_if(items.begin(), items.end(),
                                   [&item](const Repository& repo) {
                                       return item->name == repo.name;
                                   }),
                    items.end());
        EngineConfig::GetInstance().SaveConfig();

        return RemoveDialogResult_REMOVED;
    }

    ImGui::SameLine();

    if (ImGui::Button("No")) {
        return RemoveDialogResult_CANCELLED;
    }

    return RemoveDialogResult_NONE;
}

#pragma endregion

void PackageManagerOptions::Draw() {
    ImGui::Begin(ICON_CI_SETTINGS_GEAR " Package Manager Settings", &visible);

    _tableView.Draw();

    ImGui::End();
}

void PackageManager::Draw() {
    const float BOTTOM_BAR_HEIGHT = 22.0f;

    ImGui::SetNextWindowSize(ImVec2(1080, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_CI_PACKAGE " Package Manager", &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
    if (ImGui::BeginTabBar("PackageManagerTabs")) {
        if (ImGui::BeginTabItem("All")) {
            drawSplit(PackageManagerTab_ALL, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Installed")) {
            drawSplit(PackageManagerTab_INSTALLED, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Available")) {
            drawSplit(PackageManagerTab_AVAILABLE, BOTTOM_BAR_HEIGHT);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    drawBottomBar(BOTTOM_BAR_HEIGHT);

    ImGui::End();
}

void PackageManager::drawLeftPane(PackageManagerTab tab) {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static char search_str[256] = {0};

    ImGui::SetNextItemWidth(area.x);

    ImGui::InputTextWithHint(itemId("##PackageManagerSearch", tab).c_str(), ICON_CI_SEARCH " Search", search_str, 256);
    float search_bar_height = ImGui::GetItemRectSize().y;

    std::string query = search_str;
    std::unordered_map<std::string, Packages::Package> packages{};
    if (!query.empty()) {
        // Split query
        std::istringstream queryStream(query);
        std::vector<std::string> queryKeywords{};
        std::string keyword;

        while (queryStream >> keyword) queryKeywords.push_back(keyword);

        for (const auto& queryKw: queryKeywords) {
            for (const auto& [key, pkgs]: _keywordIndex) {
                if (key.find(queryKw) != std::string::npos) {
                    for (const auto& pkg: pkgs) {
                        packages[pkg->Name] = *pkg;
                    }
                }
            }
        }
    } else {
        packages = _packagesByName;
    }

    ImVec2 listbox_area = ImVec2(area.x, area.y - search_bar_height - ImGui::GetStyle().ItemSpacing.y);
    if (tab == PackageManagerTab_ALL) {
        _listView.SetupListView(itemId("##PackageManagerList", tab), listbox_area);
        _listView.Draw();
    } else {
        if (ImGui::BeginListBox(itemId("##PackageManagerList", tab).c_str(), listbox_area)) {
            for (const auto& pkg: packages) {
                ImGui::PushID(pkg.first.c_str());
                ImGui::Text("%s - %s", pkg.first.c_str(), pkg.second.Repo.c_str());
                ImGui::Text(pkg.second.Authors[0].c_str());
                // ImGui::Checkbox();
                ImGui::PopID();
                ImGui::Separator();
            }
            ImGui::EndListBox();
        }
    }
}

void PackageManager::drawRightPane(PackageManagerTab tab) {
    auto selectedItem = _listView.GetHighlightedItem();
    if (selectedItem == nullptr) {
        ImGui::Text("No selected package");
    } else {
        if (selectedItem->IsSelected()) {
            if (ImGui::Button("Unmark")) {
                selectedItem->SetSelected(false);
            }
        } else {
            if (ImGui::Button("Mark to Install")) {
                selectedItem->SetSelected(true);
            }
        }

        ImGui::Text("%s", selectedItem->Name.c_str());

        auto readmePath = selectedItem->LocalPath / "README.md";
        if (std::filesystem::exists(readmePath)) {
            std::ifstream readmeFile(readmePath);
            std::stringstream buffer;
            buffer << readmeFile.rdbuf();

            auto& currentUrl = _currentUrl;

            Components::MarkdownParser mdParser(buffer.str(), Components::MarkdownParserOptions{
                                                                      .BaseDir = selectedItem->LocalPath,
                                                                      .ImgMaxWidth = 200,
                                                                      .ImgMaxHeight = -1,
                                                                      .LinkCallback = [&currentUrl](std::string url) {
                                                                          currentUrl = std::move(url);
                                                                          ImGui::OpenPopup("Open Link?##PackageManagerOpenLink");
                                                                      }});
            mdParser.Render();
        } else {
            ImGui::Text("No README provided");
        }
    }

    if (ImGui::BeginPopupModal("Open Link?##PackageManagerOpenLink", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Open this link?");
        ImGui::Text("%s", _currentUrl.c_str());

        if (ImGui::Button("Yes")) {
            Marmalade::Util::OpenLink(_currentUrl);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("No")) ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

void PackageManager::drawSplit(PackageManagerTab tab, float bottom_bar_height) {
    ImVec2 area = ImGui::GetContentRegionAvail();

    static float split_ratio = 0.3f;
    static float min_size = 50.0f;

    float left_width = area.x * split_ratio;
    float right_width = area.x - left_width - 4.0f;

    left_width = ImMax(left_width, min_size);
    right_width = ImMax(right_width, min_size);

    ImGui::BeginChild("PackageManagerLeftPane", ImVec2(left_width, area.y - bottom_bar_height), true);
    drawLeftPane(tab);
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::InvisibleButton("##PackageManagerSplitter", ImVec2(8.0f, area.y - bottom_bar_height), ImGuiButtonFlags_None);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }

    // Handle resizing
    if (ImGui::IsItemActive()) {
        ImGuiIO& io = ImGui::GetIO();
        split_ratio += io.MouseDelta.x / area.x;
        split_ratio = ImClamp(split_ratio, min_size / area.x, 1.0f - min_size / area.x);
    }

    // Right Pane
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("PackageManagerRightPane", ImVec2(right_width, area.y - bottom_bar_height), true);
    drawRightPane(tab);
    ImGui::EndChild();
}

void PackageManager::drawBottomBar(float height) {
    float window_width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("PackageManagerBottomBar", ImVec2(window_width, height));

    if (ImGui::Button("Refresh Local Database")) {
        std::thread thread(&PackageManager::updateLocalDatabase, this);
        thread.detach();
    }

    ImGui::SameLine();
    if (ImGui::Button("Delete Local Database")) {
        deleteLocalDatabase();
    }

    ImGui::SameLine();
    if (ImGui::Button("Options")) {
        _options.ToggleWindow();
    }

    _options.Show();

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImGui::SameLine();
    ImGui::ProgressBar(_progressIndeterminate ? ImGui::GetTime() * -0.25f : _progress, ImVec2(200.0f, 0.0f), _progressText.c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine(window_width - 50);

    ImGui::BeginDisabled(!_listView.GetGlobalDirty());
    ImGui::Button("Apply");
    ImGui::EndDisabled();

    ImGui::EndChild();
}

std::string PackageManager::itemId(const char* id, PackageManagerTab tab) {
    std::stringstream item_id;
    item_id << "id" << tab;
    return item_id.str();
}

static int sideband_progress(const char* str, int len, void* payload) {
    auto* packageManager = static_cast<PackageManager*>(payload);
    packageManager->SetProgressText("Waiting for Remote");

    spdlog::debug("Remote: {}", std::string_view(str, len));
    return 0;
}

static int fetch_progress(const git_indexer_progress* stats, void* payload) {
    auto* packageManager = static_cast<PackageManager*>(payload);
    packageManager->SetProgressText("Fetching");

    spdlog::debug("Fetch progress, {} / {}", stats->received_objects, stats->total_objects);
    return 0;
}

static void checkout_progress(const char* path, size_t cur, size_t tot, void* payload) {
    auto* packageManager = static_cast<PackageManager*>(payload);
    packageManager->SetProgressText("Checking out");

    spdlog::debug("Checkout progress: {} / {}", cur, tot);
}

void PackageManager::handleGitError(const std::string& operation) {
    const git_error* err = git_error_last();
    if (err) {
        spdlog::error("GIT ERROR ({}): {}, {}", operation, err->klass, err->message);
    } else {
        spdlog::error("GIT ERROR ({}): no info", operation);
    }
}

void PackageManager::updateLocalDatabase() {
    if (_dbOperationRunning.exchange(true)) {
        spdlog::info("Another database operation is in progress. Skipping.");
        return;
    }

    _packagesByName.clear();
    _keywordIndex.clear();

    for (const auto& repo: Marmalade::EngineConfig::GetStoredConfig().repos) {
        bool no_err{false};

        if (std::filesystem::exists(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / repo.name)) {
            no_err = pullRepo(repo);
        } else {
            no_err = cloneRepo(repo);
        }

        if (no_err) buildIndex(repo);
    }

    _progressIndeterminate = false;
    _progressText = "";
    _dbOperationRunning = false;
}

bool PackageManager::cloneRepo(const Repository& config_repo) {
    git_libgit2_init();

    _progressIndeterminate = true;

    git_repository* cloned_repo = nullptr;

    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.progress_payload = this;
    checkout_opts.progress_cb = checkout_progress;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    clone_opts.checkout_opts = checkout_opts;
    clone_opts.fetch_opts.callbacks.payload = this;
    clone_opts.fetch_opts.callbacks.sideband_progress = sideband_progress;
    clone_opts.fetch_opts.callbacks.transfer_progress = &fetch_progress;
    clone_opts.fetch_opts.depth = config_repo.depth;

    int error = git_clone(&cloned_repo, config_repo.gitUrl.c_str(), (Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.name).string().c_str(), &clone_opts);
    if (error != 0) {
        handleGitError("clone");
        return false;
    }

    git_repository_free(cloned_repo);

    return true;
}

bool PackageManager::pullRepo(const Repository& config_repo) {
    git_libgit2_init();

    spdlog::info("Beginning pull for {}", config_repo.name);

    // Delete index first
    if (std::filesystem::remove(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.name / INDEX_FILENAME)) {
        spdlog::info("Index deleted successfully.");
    }

    spdlog::debug("Opening local repository");
    git_repository* repo = nullptr;
    int error = git_repository_open(&repo, (Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.name).string().c_str());
    if (error != 0) {
        handleGitError("open");
        return false;
    }

    spdlog::debug("Looking up remote: origin");
    git_remote* remote = nullptr;
    error = git_remote_lookup(&remote, repo, "origin");
    if (error != 0) {
        handleGitError("remote_lookup");
        git_repository_free(repo);
        return false;
    }

    spdlog::debug("Fetching from remote");
    error = git_remote_fetch(remote, nullptr, nullptr, nullptr);
    if (error != 0) {
        handleGitError("fetch");
        git_remote_free(remote);
        git_repository_free(repo);
        return false;
    }

    spdlog::debug("Retrieving current branch");
    git_reference* head = nullptr;
    error = git_repository_head(&head, repo);
    if (error != 0) {
        handleGitError("head");
        git_remote_free(remote);
        git_repository_free(repo);
        return false;
    }

    spdlog::debug("Looking up current branch");
    git_reference* branch = nullptr;
    error = git_branch_lookup(&branch, repo, git_reference_shorthand(head), GIT_BRANCH_LOCAL);
    if (error != 0) {
        handleGitError("branch_lookup");
        git_reference_free(head);
        git_remote_free(remote);
        git_repository_free(repo);
        return false;
    }

    spdlog::debug("Retrieving latest upstream reference");
    git_reference* upstream = nullptr;
    error = git_branch_upstream(&upstream, branch);
    if (error != 0) {
        handleGitError("branch_upstream");
        git_reference_free(branch);
        git_reference_free(head);
        git_remote_free(remote);
        git_repository_free(repo);
        return false;
    }

    git_reference_free(upstream);
    git_reference_free(branch);
    git_reference_free(head);
    git_remote_free(remote);
    git_repository_free(repo);

    spdlog::info("Pull complete");
    return true;
}

void PackageManager::buildIndex(const Repository& config_repo) {
    spdlog::info("Building index for {}...", config_repo.name);

    nlohmann::json indexJson;

    for (const auto& letterDir: std::filesystem::directory_iterator(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.name)) {
        if (!std::filesystem::is_directory(letterDir)) continue;
        if (letterDir.path().filename().string() == ".git") continue;

        spdlog::debug("Scanning directory: {}", letterDir.path().filename().string());

        for (const auto& packageDir: std::filesystem::directory_iterator(letterDir)) {
            if (!std::filesystem::is_directory(packageDir)) continue;
            spdlog::debug("Found package: {}", packageDir.path().filename().string());

            std::filesystem::path packageJsonPath = packageDir.path() / "info.json";
            if (!std::filesystem::exists(packageJsonPath)) continue;

            try {
                std::ifstream packageFile(packageJsonPath);
                nlohmann::json packageData;
                packageFile >> packageData;

                if (packageData.contains("name")) {
                    std::string packageName = packageData["name"];
                }

                Packages::Package package{packageData["name"], config_repo.name, packageData["authors"], packageData["keywords"], packageDir.path()};

                _packagesByName[package.Name] = package;
                for (const std::string& keyword: package.Keywords) {
                    _keywordIndex[keyword].push_back(&_packagesByName[package.Name]);
                }

            } catch (const std::exception& e) {
                spdlog::error("Error reading {}: {}", packageJsonPath.string(), e.what());
            }
        }
    }

    indexJson["windows"] = _packagesByName;
    indexJson["index"] = [&]() {
        std::unordered_map<std::string, std::vector<std::string>> transformed;
        for (const auto& [key, pkgPtrs]: _keywordIndex) {
            for (const Packages::Package* pkgPtr: pkgPtrs) {
                if (pkgPtr) transformed[key].push_back(pkgPtr->Name);
            }
        }
        return transformed;
    }();

    _allPackages.clear();
    for (auto& package: _packagesByName) {
        _allPackages.push_back(package.second);
    }


    // Write index.json
    std::ofstream outFile(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.name / INDEX_FILENAME);
    outFile << indexJson;

    spdlog::info("Index built successfully.");
}

void PackageManager::deleteLocalDatabase() {
    if (std::filesystem::exists(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH)) {
        try {
            std::filesystem::remove_all(Marmalade::ConfigUtil::GetConfigDirectory() / LOCAL_REPO_PATH);
            spdlog::info("Successfully deleted local database");
        } catch (const std::filesystem::filesystem_error& e) {
            spdlog::error("Failed to delete local database: {}", e.what());
        }
    } else {
        spdlog::info("Local database does not exist.");
    }
}
