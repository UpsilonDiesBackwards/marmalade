
#include "projectwizard.h"
#include "ImGuiFileDialog.h"
#include "spdlog/spdlog.h"
#include "../../application/project.h"
#include "../../application/application.h"

#include <filesystem>

#include <imgui.h>

#include <IconsCodicons.h>
#include <git2/repository.h>
#include <git2/global.h>
#include <git2/remote.h>

bool directoryExists = false;

void Marmalade::GUI::ProjectWizard::Draw() {
    char filePathC[256]; // We can not use strings in ImGui text fields therefore we gotta copy them into a char[]
    std::strcpy(filePathC, projectFilePath.c_str());

    char projectNameC[256];
    std::strcpy(projectNameC, projectName.c_str());

    char gitRemoteNameC[256];
    std::strcpy(gitRemoteNameC, gitSettings.remoteName.c_str());

    char gitRemotePathC[256];
    std::strcpy(gitRemotePathC, gitSettings.remoteURL.c_str());

    ImGui::Begin(ICON_CI_FILE_TEXT " New Project", &visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::InputText("Project Name##", projectNameC, sizeof(projectNameC))) {
        projectName = projectNameC;
    }

    if (strlen(projectNameC) == 0) { // If the project name is empty, return an error
        ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                           "Invalid Project Name!");
    }

    if (ImGui::InputText("Project Path##", filePathC, sizeof(filePathC))) {
        projectFilePath = filePathC;
    }

    ImGui::SameLine();

    ImGui::SetNextWindowPos(ImVec2(960, 540), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(720, 480), ImGuiCond_FirstUseEver);

    if (ImGui::Button(ICON_CI_FOLDER)) { // Create the ImGui File Dialog config and open it
        IGFD::FileDialogConfig config;
        config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseProjectPath", "Choose Project Directory", nullptr);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseProjectPath")) { // Display the file dialogue
        if (ImGuiFileDialog::Instance()->IsOk()) { // If the 'ok' button is clicked, set the filepath to the selected directory
            projectFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            projectFilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    if (strlen(filePathC) == 0) { // If the project path is empty, return an error
        ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                           "Invalid File Path!");
    }

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 125.0f);

    if (ImGui::Button("Create Project")) {
        CreateProject();
    }

    if (ImGui::Checkbox("Initialise Git", &gitSettings.initGitRepository)) { };

    if (gitSettings.initGitRepository) {
        ImGui::Separator();

        if (ImGui::Checkbox("Use recommended .gitignore", &gitSettings.useDefaultGitIgnore));

        if (ImGui::InputText("Remote Name##", gitRemoteNameC, sizeof(gitRemoteNameC))) {
            gitSettings.remoteName = gitRemoteNameC;
        }

        if (ImGui::InputText("Remote URL##", gitRemotePathC, sizeof(gitRemotePathC))) {
            gitSettings.remoteURL = gitRemotePathC;
        }
    }

    if (strlen(gitRemoteNameC) == 0 && gitSettings.initGitRepository) { // If the git remote name is empty, return a warning
        ImGui::TextColored(ImVec4(0.85f, 0.73f, 0.49f, 1.0f),
                           "Empty Remote Name, will default to 'main'");
    }

    if (strlen(gitRemotePathC) == 0 && gitSettings.initGitRepository) { // If the git remote url is empty, return a warning
        ImGui::TextColored(ImVec4(0.85f, 0.73f, 0.49f, 1.0f),
                           "Empty remote URL, this will require manual setup!");
    }

    if (directoryExists) { // If the desired directory already exists, return an error
        ImGui::TextColored(ImVec4(0.90f, 0.49f, 0.50f, 1.0f),
                           "Directory already exists!");
    }

    ImGui::End();
}

void Marmalade::GUI::ProjectWizard::CreateProject() {
    if (projectFilePath.empty() || projectName.empty()) return; // Ensure the file path / name fields are

    std::filesystem::path projectPath = std::filesystem::path(projectFilePath) / projectName;

    if (exists(projectPath)) { // Check if the desired directory exists
         directoryExists = true;
    } else { directoryExists = false; }

    if (std::filesystem::create_directory(projectPath)) { // Create the project directory
        spdlog::info("Creating project: {}. Using Git: {}", projectName, gitSettings.initGitRepository);

        // TODO: Project setup

        Application::GetInstance().SetCurrentProject(std::make_unique<Project>(projectName, projectPath.string(), gitSettings));

        if (gitSettings.initGitRepository) { // If desired then initialise a git repo at the project directory
            InitialiseGitRepository(projectPath.string().c_str());
        }

        projectName = "";
        visible = false;
    }
}

void Marmalade::GUI::ProjectWizard::InitialiseGitRepository(const char* repoPath) {
    git_libgit2_init();

    int error;
    git_repository_init_options options = GIT_REPOSITORY_INIT_OPTIONS_INIT;

    options.flags |= GIT_REPOSITORY_INIT_MKPATH;
    options.description = "Marmalade Project"; // Repo description, perhaps this should be user defined?

    error = git_repository_init_ext(&gitSettings.repo, repoPath, &options); // Create the repo

    if (gitSettings.useDefaultGitIgnore) { // Use .gitignore template or not
        std::filesystem::path targetIgnorePath = std::filesystem::path(projectFilePath) / projectName / ".gitignore";

        std::ifstream templIgnore("../resources/templates/gitignore"); // Template ignore
        std::ofstream targetIgnore(targetIgnorePath);

        if (templIgnore.is_open() && targetIgnore.is_open()) { // If both files are open ...
            std::string line;

            while (std::getline(templIgnore, line)) { // ... copy each line to the target .gitignore
                targetIgnore << line << "\n";
            }

            templIgnore.close();
            targetIgnore.close();
        } else {
            spdlog::error("Failed copying .gitignore contents!");
        }
    }

    if (!gitSettings.remoteURL.length() == 0) {
        SetGitRemoteURL(gitSettings);
    }

    // TODO: Git error handling

    git_libgit2_shutdown();
}

void Marmalade::GUI::ProjectWizard::SetGitRemoteURL(GitSettings git) {
    git_libgit2_init();

    int error;
    error = git_remote_set_url(git.repo, git.remoteName.c_str(), git.remoteURL.c_str());

    spdlog::error("Git error: {}", error);


    // TODO: Git error handling

    git_libgit2_shutdown();
}
