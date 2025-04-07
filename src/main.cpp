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

#include "application/application.h"
#include "application/config/configutil.h"
#include "application/config/recents.h"
#include "application/config/plugins.h"
#include "application/pluginloader.h"
#include "gui/windowmanager.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32

#include <windows.h>

#define ARGC __argc
#define ARGV __argv

#else

#define ARGC argc
#define ARGV argv

#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    bool sameDirConfig{false};
    char* project = nullptr;

    for (int i = 1; i < ARGC; ++i) {
        std::string arg = ARGV[i];

        if (arg == "--same-dir-config") {
            sameDirConfig = true;
        }

        if (arg == "--project") {
            if (ARGC > i) {
                project = ARGV[i + 1];
                i++;
            }
        }
    }

    if (project != nullptr) {
        std::cout << "Chosen project" << project << std::endl;
    } else {
        std::cout << "No chosen project" << std::endl;
    }

    Marmalade::ConfigUtil::SetConfigDirectory(sameDirConfig);
    if (!Marmalade::EngineConfig::GetInstance().LoadConfig()) {
        return 1;
    }
    if (!Marmalade::Recents::GetInstance().LoadConfig()) {
        Marmalade::Recents::GetInstance().RecreateConfig();
    }
    if (!Marmalade::Plugins::GetInstance().LoadConfig()) {
        Marmalade::Plugins::GetInstance().RecreateConfig();
    }

    Application& application = Application::GetInstance(1920, 1080, "Marmalade Engine");
    application.Initialise();

    // Load plugins
    Marmalade::PluginLoader::GetInstance().LoadPlugins();

    if (project != nullptr) {
        // Open specified project
        if (application.OpenProject(project)) {
            Marmalade::GUI::WindowManager::GetInstance().welcomeScreen.visible = false;
        }
    }

    while (!glfwWindowShouldClose(application.getWindow())) {
        application.Run();
    }

    application.OnClose();

    Marmalade::PluginLoader::GetInstance().UnloadPlugins();
    application.Terminate();
    return 0;
}
