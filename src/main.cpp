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
#include "application/config.h"
#include "application/recents.h"
#include "application/plugins.h"
#include "application/pluginloader.h"
#include "gui/windowmanager.h"

#include <iostream>

int main(int argc, char** argv) {
    bool sameDirConfig{false};
    char* project = nullptr;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--same-dir-config") {
            sameDirConfig = true;
        }

        if (arg == "--project") {
            if (argc > i) {
                project = argv[i + 1];
                i++;
            }
        }
    }

    if (project != nullptr) {
        std::cout << "Chosen project" << project << std::endl;
    } else {
        std::cout << "No chosen project" << std::endl;
    }

    Marmalade::Config::SetConfigDirectory(sameDirConfig);
    Marmalade::Config::LoadEngineConfig();
    Marmalade::Recents::LoadRecents();
    Marmalade::Plugins::LoadPlugins();

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
