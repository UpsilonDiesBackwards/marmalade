
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

#include "versioncontrol.h"

#include "../../application/application.h"
#include "../../application/plugins/interfaceimpl.h"

#include <git2/global.h>
#include <git2/repository.h>
#include <git2/status.h>
#include <git2/diff.h>
#include <git2/blob.h>
#include <git2/index.h>
#include <git2/errors.h>

#include <imgui.h>

#include <IconsCodicons.h>

namespace fs = std::filesystem;

void Marmalade::GUI::VersionControl::Draw() {
    WINDOW_BEGIN(ICON_CI_GIT_COMMIT " Version Control", ImGuiWindowFlags_None)

    if (Application::GetInstance().GetCurrentProject()->CheckIfGitRepository()) { // If the current project a valid git repo...
        CheckForModifiedItems(); //... then check for modified items...
    } else { //... or just, don't
        ImGui::End();
        return;
    }

    DisplayModifiedItems();

    if (_showDiffViewer) {
        ShowItemDiff(oldContent, newContent);
    }

    WINDOW_END()
}

void Marmalade::GUI::VersionControl::CheckForModifiedItems() {
    static bool libGit2Init = false;
    if (!libGit2Init) {
        git_libgit2_init();
        libGit2Init = true;
    }

    // Open the Git repository if it is not open already
    if (repo == nullptr) {
        if (git_repository_open(&repo, Application::GetInstance().GetCurrentProject()->basePath.string().c_str()) != 0) {
            LOG_ERROR("Failed to open Git repository: {}", git_error_last()->message);
            return;
        }
    }

    // Create status list for the repository
    if (git_status_list_new(&statusList, repo, nullptr) != 0) {
        LOG_ERROR("Failed to get status list for repository: {}", git_error_last()->message);
        return;
    }

    _modifiedFiles.clear(); // Clear the modified files vector just to ensure its clean

    size_t statusCount = git_status_list_entrycount(statusList);

    for (size_t i = 0; i < statusCount; ++i) { // For each item in the status list...
        const git_status_entry *entry = git_status_byindex(statusList, i);

        if (entry->index_to_workdir != nullptr) { //... check if there are changes in between index and working versions
            const char *filename = entry->index_to_workdir->new_file.path;
            std::string fullPath = (Application::GetInstance().GetCurrentProject()->basePath / filename).string();

            if (entry->status & GIT_STATUS_WT_MODIFIED) { //... if there are changes, push its filepath to the _modifiedFiles vector
                _modifiedFiles.push_back(fullPath);
            }
        } else { // ... check if there are files between head and index (such as new or deleted files)...
            if (entry->head_to_index != nullptr) {
                const char *filename = entry->head_to_index->old_file.path;
                std::string fullPath = (Application::GetInstance().GetCurrentProject()->basePath / filename).string();

                if (entry->status & GIT_STATUS_INDEX_NEW) { //... if the file is new or deleted, push its filepath to the vector like before
                    _modifiedFiles.push_back(fullPath);
                } else if (entry->status & GIT_STATUS_INDEX_DELETED) {
                    _modifiedFiles.push_back(fullPath);
                }
            }
        }
    }

    git_status_list_free(statusList);
    statusList = nullptr;
}

void Marmalade::GUI::VersionControl::GetItemDiff(const std::string file) {
    git_diff *itemDiff = nullptr;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;

    if (repo == nullptr) {
        LOG_ERROR("Repository is not initialized properly: {}", git_error_last()->message);
        return;
    }

    git_reference * headRef = nullptr;
    int err = git_repository_head(&headRef, repo);
    if (err != 0 || headRef == nullptr) { // Attempt to get most recent commit
        LOG_ERROR("Failed to lookup HEAD commit, error: {}", git_error_last()->message);
        return;
    }

    const git_oid * headOid = git_reference_target(headRef); // Get the OID (id) of the commit
    if (headOid == nullptr) {
        git_reference_free(headRef);
        return;
    }

    git_commit * headCommit = nullptr;
    if (git_commit_lookup(&headCommit, repo, headOid) != 0) { // Lookup that commit object using the OID
        LOG_ERROR("Failed to lookup HEAD commit, error: {}", git_error_last()->message);
        git_reference_free(headRef);
        return;
    }

    git_reference_free(headRef);

    git_tree *tree = nullptr;
    if (git_commit_tree(&tree, headCommit) != 0) { // Get the file tree of that commit
        LOG_ERROR("Failed to get tree from HEAD commit: {}", git_error_last()->message);
        git_commit_free(headCommit);
        return;
    }

    err = git_diff_index_to_workdir(&itemDiff, repo, nullptr, &opts); // Create a diff between the index and working directory
    if (err < 0) {
        LOG_ERROR("Failed to generate diff: {}", git_error_last()->message);
        git_tree_free(tree);
        git_commit_free(headCommit);
        return;
    }

    size_t numDeltas = git_diff_num_deltas(itemDiff);
    for (size_t i = 0; i < numDeltas; ++i) { // Loop through each file change (deltas)
        const git_diff_delta *delta = git_diff_get_delta(itemDiff, i);
        const char * oldFile = delta->old_file.path;
        const char * newFile = delta->new_file.path;

        std::string relativeFile = std::filesystem::relative(file, Application::GetInstance().GetCurrentProject()->basePath).string();

        if (strcmp(relativeFile.c_str(), newFile) == 0 || strcmp(relativeFile.c_str(), oldFile) == 0) { // Check if the file is the same file
            oldContent = GetOldVersionLines(file); // Get old...
            newContent = GetNewVersionLines(file); //... and new content of the file

            _showDiffViewer = true;
        }

    }

    git_diff_free(itemDiff);
    git_tree_free(tree);
    git_commit_free(headCommit);
}


void Marmalade::GUI::VersionControl::DisplayModifiedItems() {
    if (_modifiedFiles.empty()) {
        ImGui::Text("No modified files.");
        return;
    }

    for (const auto& file : _modifiedFiles) { // Loop through each file in the file vector and display them
        if (ImGui::Selectable(file.c_str())) { // If an item is selected, then get its diff
            _selectedFile = file;

            GetItemDiff(_selectedFile);
        }
    }
}

void Marmalade::GUI::VersionControl::ShowItemDiff(const std::vector<std::string>& oldContent, const std::vector<std::string>& newContent) {
    ImGui::Begin("Diff Viewer");

    if (ImGui::Button("Close")) {
        _showDiffViewer = false;
    }

    ImGui::Columns(2);

    ImGui::Text("HEAD");
    ImGui::NextColumn();
    ImGui::Text("LOCAL");
    ImGui::NextColumn();

    size_t maxLines = std::max(oldContent.size(), newContent.size());
    for (size_t i = 0; i < maxLines; ++i) { // Loop through each line between the two vectors...
        ImGui::PushID(i);

        if (i < oldContent.size()) { //... for the old content...
            const std::string& oldLine = oldContent[i];
            ImVec4 color = ImVec4(1, 1, 1, 1); //... if the line has not been modified, then color it white...

            if (i >= newContent.size() || oldLine != newContent[i]) {
                color = ImVec4(1, 0, 0, 1);
            }

            ImGui::TextColored(color, "%s", oldLine.c_str()); //... if it has, color it red...
        } else {
            ImGui::Text("");
        }

        ImGui::NextColumn();

        if (i < newContent.size()) { //... for the new content...
            const std::string& newLine = newContent[i];
            ImVec4 color = ImVec4(1, 1, 1, 1); //... if it has not been modified, color it white...

            if (i >= oldContent.size() || newLine != oldContent[i]) { //... if it has been modified, color it green
                color = ImVec4(0, 1, 0, 1);
            }

            ImGui::TextColored(color, "%s", newLine.c_str());
        } else {
            ImGui::Text("");
        }

        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::End();
}

std::vector<std::string> Marmalade::GUI::VersionControl::GetOldVersionLines(const std::string &file_path) {
    std::vector<std::string> lines;

    git_oid headOid;
    if (git_reference_name_to_id(&headOid, repo, "HEAD") != 0) { // Get HEAD commit...
        LOG_ERROR("Failed to get HEAD reference: {}", git_error_last()->message);
        return lines;
    }

    git_commit * headCommit = nullptr;
    if (git_commit_lookup(&headCommit, repo, &headOid) != 0) { //... then look it up
        LOG_ERROR("Failed to lookup HEAD commit: {}", git_error_last()->message);
        return lines;
    }

    git_tree *tree = nullptr;
    if (git_commit_tree(&tree, headCommit) != 0) { //... get the file tree at that commit
        LOG_ERROR("Failed to get tree from commit: {}", git_error_last()->message);
        git_commit_free(headCommit);
        return lines;
    }

    std::filesystem::path relative_path = std::filesystem::relative(file_path, Application::GetInstance().GetCurrentProject()->basePath);

    git_tree_entry *entry = nullptr;
    if (git_tree_entry_bypath(&entry, tree, relative_path.string().c_str()) != 0) { // Get file at that tree entry
        LOG_ERROR("Failed to get file from tree: {}", git_error_last()->message);
        git_tree_free(tree);
        git_commit_free(headCommit);
        return lines;
    }

    git_blob *blob = nullptr;
    if (git_blob_lookup(&blob, repo, git_tree_entry_id(entry)) != 0) { // Get the blob of that file tree entry
        LOG_ERROR("Failed to lookup blob: {}", git_error_last()->message);
        git_tree_entry_free(entry);
        git_tree_free(tree);
        git_commit_free(headCommit);
        return lines;
    }

    const char *fileContent = (const char*)git_blob_rawcontent(blob); // Then get its file contents
    std::stringstream ss(fileContent);
    std::string line;
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    git_blob_free(blob);
    git_tree_entry_free(entry);
    git_tree_free(tree);
    git_commit_free(headCommit);

    return lines;
}

std::vector<std::string> Marmalade::GUI::VersionControl::GetNewVersionLines(const std::string &file_path) {
    std::vector<std::string> lines;

    git_index *index = nullptr;
    if (git_repository_index(&index, repo) != 0) { // Get repository index
        LOG_ERROR("Failed to get repository index: {}", git_error_last()->message);
        return lines;
    }

    std::filesystem::path relative_path = std::filesystem::relative(file_path, Application::GetInstance().GetCurrentProject()->basePath);

    git_index_entry *entry = const_cast<git_index_entry*>(git_index_get_bypath(index, relative_path.string().c_str(), 0)); // Get file from index...
    if (!entry) {
        LOG_ERROR("Failed to get file from index: {}", git_error_last()->message);
        git_index_free(index);
        return lines;
    }

    git_blob *blob = nullptr;
    if (git_blob_lookup(&blob, repo, &entry->id) != 0) { //... then find its blob
        LOG_ERROR("Failed to lookup blob: {}", git_error_last()->message);
        git_index_free(index);
        return lines;
    }

    const char *fileContent = (const char*)git_blob_rawcontent(blob);  // Then get its file contents
    std::stringstream ss(fileContent);
    std::string line;
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    git_blob_free(blob);
    git_index_free(index);

    return lines;
}
