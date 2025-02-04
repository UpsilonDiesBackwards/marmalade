
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

bool directoryExists = false;

void Marmalade::GUI::ProjectWizard::Draw() {
    char filePathC[256]; // We can not use strings in ImGui text fields therefore we gotta copy them into a char[]
    std::strcpy(filePathC, projectFilePath.c_str());

    char projectNameC[256];
    std::strcpy(projectNameC, projectName.c_str());

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

    if (ImGui::Checkbox("Initialise Git", &initGitRepository)) { };

    ImGui::SameLine();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 125.0f);

    if (ImGui::Button("Create Project")) {
        CreateProject();
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
        spdlog::info("Creating project: {}. Using Git: {}", projectName, initGitRepository);

        // TODO: Project setup
        Project project(projectName, projectPath, initGitRepository);

        Application::GetInstance().SetCurrentProject(project);

        if (initGitRepository) { // If desired then initialise a git repo at the project directory
            InitialiseGitRepository(projectPath.c_str());
        }

        projectName = "";
        visible = false;
    }
}

void Marmalade::GUI::ProjectWizard::InitialiseGitRepository(const char* repoPath) {
    git_libgit2_init();

    int error;
    git_repository *repo = NULL;
    git_repository_init_options options = GIT_REPOSITORY_INIT_OPTIONS_INIT;

    options.flags |= GIT_REPOSITORY_INIT_MKPATH;
    options.description = "Marmalade Project"; // Repo description, perhaps this should be user defined?

    error = git_repository_init_ext(&repo, repoPath, &options); // Create the repo

    // TODO: Git error handling

    git_libgit2_shutdown();
}
