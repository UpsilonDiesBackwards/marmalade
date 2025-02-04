#include "packagemanager.h"

#include <sstream>
#include <thread>
#include <filesystem>
#include <fstream>

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

#include <git2.h>

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#define LOCAL_REPO_PATH "local-repo"
#define INDEX_FILENAME "index.json"

using namespace Marmalade::GUI;

void PackageManagerOptions::Draw() {
    ImGui::Begin(ICON_CI_SETTINGS_GEAR " Package Manager Settings", &visible);

    if (ImGui::Button("Add")) {
        _isEditing = true;
        _currentRepo = nullptr;
    }

    ImGui::SameLine();
    auto cursor_pos = ImGui::GetCursorPos();
    ImGui::NewLine();

    if (ImGui::BeginTable("PackageManagerReposTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("URL", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();

        int i = 0;
        for (const auto& repo: Marmalade::Config::engineConfig.Repos) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            bool is_selected = (_selectedRow == i);
            if (ImGui::Selectable(repo.Name.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                _selectedRow = (is_selected ? -1 : i);
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", repo.GitUrl.c_str());

            i++;
        }

        ImGui::EndTable();
    }

    if (_selectedRow > -1) {
        Marmalade::Repository& sel_item = Marmalade::Config::engineConfig.Repos[_selectedRow];

        ImGui::SetCursorPos(cursor_pos);

        if (ImGui::Button("Edit")) {
            _isEditing = true;
            _currentRepo = &sel_item;
            std::strncpy(_tempName, _currentRepo->Name.c_str(), sizeof(_tempName) - 1);
            std::strncpy(_tempGitUrl, _currentRepo->GitUrl.c_str(), sizeof(_tempGitUrl) - 1);
            _tempDepth = _currentRepo->Depth;
        }
        ImGui::SameLine();

        if (ImGui::Button("Remove")) {
            Config::engineConfig.Repos.erase(std::remove_if(Config::engineConfig.Repos.begin(), Config::engineConfig.Repos.end(),
                                                                       [&sel_item](const Repository &repo) {
                                                                           return sel_item.Name == repo.Name;
                                                                       }), Config::engineConfig.Repos.end());
            Config::SaveEngineConfig();
            _selectedRow = -1;
        }
    }

    if (_isEditing) {
        ImGui::OpenPopup("Edit Repository##PackageManagerReposEdit");
    }

    if (ImGui::BeginPopupModal("Edit Repository##PackageManagerReposEdit")) {
        bool creating = _currentRepo == nullptr;

        ImGui::InputText("Name", _tempName, 256);
        ImGui::InputText("Git URL", _tempGitUrl, 256);
        ImGui::InputInt("Depth", &_tempDepth);

        if (ImGui::Button("Cancel")) {
            _isEditing = false;
            memset(_tempName, 0, 256);
            memset(_tempGitUrl, 0, 256);
            _tempDepth = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Save")) {
            _isEditing = false;

            if (creating) {
                Marmalade::Config::engineConfig.Repos.push_back(Repository{_tempName, _tempGitUrl, _tempDepth});
            } else {
                _currentRepo->Name = _tempName;
                _currentRepo->GitUrl = _tempGitUrl;
                _currentRepo->Depth = _tempDepth;
            }

            Marmalade::Config::SaveEngineConfig();
            memset(_tempName, 0, 256);
            memset(_tempGitUrl, 0, 256);
            _tempDepth = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void PackageManager::Draw() {
    const float BOTTOM_BAR_HEIGHT = 22.0f;

    ImGui::SetNextWindowSize(ImVec2(1080, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_CI_PACKAGE " Package Manager", &visible, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
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
    std::unordered_map<std::string, Package> packages{};
    if (!query.empty()) {
        if (_keywordIndex.count(query)) {
            for (auto pkg: _keywordIndex[query]) {
                packages[pkg->Name] = *pkg;
            }
        }
    } else {
        packages = _packagesByName;
    }

    ImVec2 listbox_area = ImVec2(area.x, area.y - search_bar_height - ImGui::GetStyle().ItemSpacing.y);
    if (ImGui::BeginListBox(itemId("##PackageManagerList", tab).c_str(), listbox_area)) {
        for (const auto& pkg: packages) {
            ImGui::PushID(pkg.first.c_str());
            ImGui::Text("%s - %s", pkg.first.c_str(), pkg.second.Repo.c_str());
            ImGui::Text(pkg.second.Author[0].c_str());
            // ImGui::Checkbox();
            ImGui::PopID();
            ImGui::Separator();
        }
        ImGui::EndListBox();
    }
}

void PackageManager::drawRightPane(PackageManagerTab tab) {
    ImGui::Text("Package details");
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
    ImGui::Button("Apply");

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

    for (const auto& repo: Marmalade::Config::engineConfig.Repos) {
        if (std::filesystem::exists(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / repo.Name)) {
            pullRepo(repo);
        } else {
            cloneRepo(repo);
        }

        buildIndex(repo);
    }

    _progressIndeterminate = false;
    _progressText = "";
    _dbOperationRunning = false;
}

void PackageManager::cloneRepo(const Repository& config_repo) {
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
    clone_opts.fetch_opts.depth = config_repo.Depth;

    int error = git_clone(&cloned_repo, config_repo.GitUrl.c_str(), (Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.Name).string().c_str(), &clone_opts);
    if (error != 0) {
        handleGitError("clone");
        return;
    }

    git_repository_free(cloned_repo);
}

void PackageManager::pullRepo(const Repository& config_repo) {
    git_libgit2_init();

    spdlog::info("Beginning pull for {}", config_repo.Name);

    // Delete index first
    if (std::filesystem::remove(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.Name / INDEX_FILENAME)) {
        spdlog::info("Index deleted successfully.");
    }

    spdlog::debug("Opening local repository");
    git_repository* repo = nullptr;
    int error = git_repository_open(&repo, (Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.Name).string().c_str());
    if (error != 0) {
        handleGitError("open");
        return;
    }

    spdlog::debug("Looking up remote: origin");
    git_remote* remote = nullptr;
    error = git_remote_lookup(&remote, repo, "origin");
    if (error != 0) {
        handleGitError("remote_lookup");
        git_repository_free(repo);
        return;
    }

    spdlog::debug("Fetching from remote");
    error = git_remote_fetch(remote, nullptr, nullptr, nullptr);
    if (error != 0) {
        handleGitError("fetch");
        git_remote_free(remote);
        git_repository_free(repo);
        return;
    }

    spdlog::debug("Retrieving current branch");
    git_reference* head = nullptr;
    error = git_repository_head(&head, repo);
    if (error != 0) {
        handleGitError("head");
        git_remote_free(remote);
        git_repository_free(repo);
        return;
    }

    spdlog::debug("Looking up current branch");
    git_reference* branch = nullptr;
    error = git_branch_lookup(&branch, repo, git_reference_shorthand(head), GIT_BRANCH_LOCAL);
    if (error != 0) {
        handleGitError("branch_lookup");
        git_reference_free(head);
        git_remote_free(remote);
        git_repository_free(repo);
        return;
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
        return;
    }

    git_reference_free(upstream);
    git_reference_free(branch);
    git_reference_free(head);
    git_remote_free(remote);
    git_repository_free(repo);

    spdlog::info("Pull complete");
}

void PackageManager::buildIndex(const Repository& config_repo) {
    spdlog::info("Building index for {}...", config_repo.Name);

    nlohmann::json indexJson;

    _packagesByName.clear();
    _keywordIndex.clear();

    for (const auto& letterDir: std::filesystem::directory_iterator(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.Name)) {
        if (!std::filesystem::is_directory(letterDir)) continue;
        if (letterDir.path().filename().string() == ".git") continue;

        spdlog::debug("Scanning directory: {}", letterDir.path().filename().string());

        for (const auto& packageDir: std::filesystem::directory_iterator(letterDir)) {
            if (!std::filesystem::is_directory(packageDir)) continue;
            spdlog::debug("Found package: {}", packageDir.path().filename().string());

            std::filesystem::path packageJsonPath = packageDir.path() / "package.json";
            if (!std::filesystem::exists(packageJsonPath)) continue;

            try {
                std::ifstream packageFile(packageJsonPath);
                nlohmann::json packageData;
                packageFile >> packageData;

                if (packageData.contains("name")) {
                    std::string packageName = packageData["name"];
                    indexJson[packageName] = packageData;
                }

                Package package{packageData["name"], config_repo.Name, packageData["authors"], packageData["keywords"]};

                _packagesByName[package.Name] = package;
                for (const std::string& keyword: package.Keywords) {
                    _keywordIndex[keyword].push_back(&_packagesByName[package.Name]);
                }

            } catch (const std::exception& e) {
                spdlog::error("Error reading {}: {}", packageJsonPath.string(), e.what());
            }
        }
    }

    // Write index.json
    std::ofstream outFile(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH / config_repo.Name / INDEX_FILENAME);
    outFile << indexJson.dump(4);

    spdlog::info("Index built successfully.");
}

void PackageManager::deleteLocalDatabase() {
    if (std::filesystem::exists(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH)) {
        try {
            std::filesystem::remove_all(Marmalade::Config::GetConfigDirectory() / LOCAL_REPO_PATH);
            spdlog::info("Successfully deleted local database");
        } catch (const std::filesystem::filesystem_error& e) {
            spdlog::error("Failed to delete local database: {}", e.what());
        }
    } else {
        spdlog::info("Local database does not exist.");
    }
}
