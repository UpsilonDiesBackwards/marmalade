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
#include "application/i18n.h"
#include "application/config/configutil.h"
#include "application/config/recents.h"
#include "application/config/plugins.h"
#include "application/plugins/pluginloader.h"
#include "gui/windowmanager.h"

#include "gui/nativeui/app.h"
#include "gui/nativeui/window.h"
#include "gui/nativeui/splashscreen.h"
#include "gui/nativeui/msgbox.h"

#include <string>

#ifdef _WIN32

#include <windows.h>

#define ARGC __argc
#define ARGV __argv

#else

#define ARGC argc
#define ARGV argv

#endif


using namespace Marmalade::GUI;

// Mutex for ensuring splash screen is created
std::mutex splashMutex;
std::condition_variable splashCV;
bool splashReady = false;

void showSplashScreen(app_handle_type_t app, NativeUI::Window& splashScreen) {
#if defined(__linux__)
    splashScreen.SetApp(app);
#endif
    splashScreen.Create(true);
    splashScreen.Show(true);

    // Allow main thread to continue
    {
        std::lock_guard<std::mutex> lock(splashMutex);
        splashReady = true;
    }
    splashCV.notify_one();
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Set working directory to same path as executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
    SetCurrentDirectoryW(exeDir.c_str());
#else

bool engineMain(bool sameDirConfig, bool noSplash, char* project, char* scene, bool secretDebugMenu, NativeUI::Window& splashScreen) {
    if (!noSplash) NativeUI::SplashScreen::SetLoadingText(splashScreen, "Loading settings...");
    Marmalade::ConfigUtil::SetConfigDirectory(sameDirConfig);
    if (!Marmalade::EngineConfig::GetInstance().LoadConfig()) {
        NativeUI::MsgBox::ShowMessage(noSplash ? nullptr : splashScreen.GetHandle(), NativeUI::Util::utf8ToUtf16Str("Failed to load settings. See log for details."), NativeUI::Util::utf8ToUtf16Str("Marmalade Engine"), NativeUI::MsgBox::Style::Style_ERROR);
        return false;
    }

    Marmalade::I18n::SetupI18n(Marmalade::EngineConfig::GetStoredConfig().appearance.language.c_str());

    if (!Marmalade::Recents::GetInstance().LoadConfig()) {
        Marmalade::Recents::GetInstance().RecreateConfig();
    }
    if (!Marmalade::Plugins::GetInstance().LoadConfig()) {
        Marmalade::Plugins::GetInstance().RecreateConfig();
    }

    if (!noSplash) NativeUI::SplashScreen::SetLoadingText(splashScreen, _("Initialising application..."));
    Application& application = Application::GetInstance(1920, 1080, _("Marmalade Engine"));
    application.Initialise();

    bool safeMode = false;
    if (NativeUI::SplashScreen::AreSafeModeKeysHeld()) {
        std::cout << "Key down";
        if (NativeUI::MsgBox::ShowMessage(noSplash ? nullptr : splashScreen.GetHandle(), NativeUI::Util::utf8ToUtf16Str(_("Would you like to enable safe mode?")), NativeUI::Util::utf8ToUtf16Str(_("Marmalade Engine")),
                                          NativeUI::MsgBox::Style::Style_INFO, NativeUI::MsgBox::Buttons::Buttons_YES_NO) == NativeUI::MsgBox::Result::Result_YES) {
            safeMode = true;
        }
    }
    application.enableDebugMenu = secretDebugMenu;

    if (!safeMode) {
        // Load plugins
        if (!noSplash) NativeUI::SplashScreen::SetLoadingText(splashScreen, _("Loading plugins..."));
        Marmalade::PluginLoader::GetInstance().LoadPlugins();
    }

    if (project != nullptr) {
        // Open specified project
        if (!noSplash) NativeUI::SplashScreen::SetLoadingText(splashScreen, _("Opening project..."));
        if (application.OpenProject(project)) {
            Marmalade::GUI::WindowManager::GetInstance().welcomeScreen.visible = false;
        }
    }

    if (scene != nullptr) {
        auto* project = GET_APP.GetCurrentProject();
        std::string sceneDir = project->basePath.string() + "/data/";
        std::string uuid = application.sceneManager.FindSceneUUIDByName(scene, sceneDir);

        if (!uuid.empty()) {
            std::string sceneFileName = Marmalade::Project::ProjectScenes::GetSceneFileName(uuid);
            Scene loadedScene = project->scenes.LoadScene(sceneFileName);
            auto scenePtr = std::make_shared<Scene>(loadedScene);
            application.sceneManager.AddScene(scenePtr);
            application.sceneManager.SetCurrentScene(uuid);
        } else {
            LOG_ERROR("Failed to load scene program argument, scene not found: {}", scene);
        }
    }


    return true;
}

int main(int argc, char** argv) {
#endif
    bool sameDirConfig{false};
    bool noSplash{false};
    char* project = nullptr;
    char* scene = nullptr;
    bool secretDebugMenu{false};

    for (int i = 1; i < ARGC; ++i) {
        std::string arg = ARGV[i];

        if (arg == "--same-dir-config") {
            sameDirConfig = true;
        }

        if (arg == "--no-splash") {
            noSplash = true;
        }

        if (arg == "--project") {
            if (ARGC > i) {
                project = ARGV[i + 1];
                i++;
            }
        }
        if (arg == "--secret-debug-menu") {
            secretDebugMenu = true;
        }

        if (arg == "--scene") {
            if (ARGC > i) {
                scene = ARGV[i + 1];
                i++;
            }
        }
    }

    auto splashScreen = NativeUI::Window(NativeUI::Util::utf8ToUtf16Str("Marmalade Engine Startup"), 800, 500);

#if defined(__APPLE__)
    auto nativeApp = std::make_shared<NativeUI::App>();
#endif

    if (!noSplash) {
        splashScreen.SetCreateCallback([&] {
            NativeUI::SplashScreen::Create(splashScreen);
        });

        splashScreen.SetPaintCallback([&] {
            NativeUI::SplashScreen::Paint(splashScreen);
        });

#if defined(__linux__)
        // Create a native app
        auto nativeApp = std::make_shared<NativeUI::App>();

        nativeApp->SetCreateCallback([&](app_handle_type_t app) {
            showSplashScreen(app, splashScreen);
        });

        // GTK application needs to be in another thread
        // This can be disregarded for other platforms
        std::thread gtkThread([&]() {
            // Don't pass any other args to GTK; GTK doesn't like them
            char* gtkArgv[] = {const_cast<char*>(ARGV[0]), nullptr};
            nativeApp->Create(1, gtkArgv);
        });
#elif defined(__APPLE__)
        nativeApp->SetCreateCallback([&](app_handle_type_t app) {
            showSplashScreen(app, splashScreen);


            std::thread loadingThread([&]() {
                engineMain(sameDirConfig, noSplash, project, scene, secretDebugMenu, splashScreen);

                if (!noSplash) splashScreen.Close();


                auto& application = Application::GetInstance();
                nativeApp->RunOnMainThread([&application, &nativeApp] {
                    application.InitialiseWindow();


                    while (!glfwWindowShouldClose(application.getWindow())) {
                        application.InitialiseImGui();
                        while (!application.NeedsImGuiRestart() && !glfwWindowShouldClose(application.getWindow())) {
                            application.Run();
                        }
                        application.TerminateImGui();
                    }

                    application.OnClose();

                    nativeApp->Terminate();

                    Marmalade::PluginLoader::GetInstance().UnloadPlugins();
                    application.TerminateGlfw();
                });
            });
            loadingThread.detach();
        });

        nativeApp->Create(ARGC, ARGV);
#else
    // Call showSplashScreen directly
    showSplashScreen(nullptr, splashScreen);
#endif

        // Wait for splash screen to be shown
        {
            std::unique_lock<std::mutex> lock(splashMutex);
            splashCV.wait(lock, [] { return splashReady; });
        }
    }


    if (!noSplash) splashScreen.Close();

    auto& application = Application::GetInstance();
    application.InitialiseWindow();
    while (!glfwWindowShouldClose(application.getWindow())) {
        application.InitialiseImGui();
        while (!application.NeedsImGuiRestart() && !glfwWindowShouldClose(application.getWindow())) {
            application.Run();
        }
        application.TerminateImGui();
    }

    application.OnClose();

    Marmalade::PluginLoader::GetInstance().UnloadPlugins();
    application.TerminateGlfw();

    if (!engineMain(sameDirConfig, noSplash, project, scene, secretDebugMenu, splashScreen)) {
        return 1;
    }

#if defined(__linux__)
    // Wait for GTK thread
    // gtkThread.join();
#endif
    return 0;
}
