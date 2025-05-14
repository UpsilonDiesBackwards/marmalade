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

    auto splashScreen = NativeUI::Window(NativeUI::Util::utf8ToUtf16Str("Marmalade Engine Startup"), 800, 500);
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
    // We should be creating a nativeApp object here
    // But that leads to a lot of threading problems on macOS...
    showSplashScreen(nullptr, splashScreen);
#else
    // Call showSplashScreen directly
    showSplashScreen(nullptr, splashScreen);
#endif

    // Wait for splash screen to be shown
    {
        std::unique_lock<std::mutex> lock(splashMutex);
        splashCV.wait(lock, [] { return splashReady; });
    }

    NativeUI::SplashScreen::SetLoadingText(splashScreen, "Loading settings...");
    Marmalade::ConfigUtil::SetConfigDirectory(sameDirConfig);
    if (!Marmalade::EngineConfig::GetInstance().LoadConfig()) {
        NativeUI::MsgBox::ShowMessage(splashScreen.GetHandle(), NativeUI::Util::utf8ToUtf16Str("Failed to load settings. See log for details."), NativeUI::Util::utf8ToUtf16Str("Marmalade Engine"), NativeUI::MsgBox::Style::Style_ERROR);
        return 1;
    }

    Marmalade::I18n::SetupI18n(Marmalade::EngineConfig::GetStoredConfig().appearance.language.c_str());

    if (!Marmalade::Recents::GetInstance().LoadConfig()) {
        Marmalade::Recents::GetInstance().RecreateConfig();
    }
    if (!Marmalade::Plugins::GetInstance().LoadConfig()) {
        Marmalade::Plugins::GetInstance().RecreateConfig();
    }

    NativeUI::SplashScreen::SetLoadingText(splashScreen, "Initialising application...");
    Application& application = Application::GetInstance(1920, 1080, "Marmalade Engine");
    application.Initialise();

    bool safeMode = false;
    if (NativeUI::SplashScreen::AreSafeModeKeysHeld()) {
        std::cout << "Key down";
        if (NativeUI::MsgBox::ShowMessage(splashScreen.GetHandle(), NativeUI::Util::utf8ToUtf16Str("Would you like to enable safe mode?"), NativeUI::Util::utf8ToUtf16Str("Marmalade Engine"),
                                          NativeUI::MsgBox::Style::Style_INFO, NativeUI::MsgBox::Buttons::Buttons_YES_NO) == NativeUI::MsgBox::Result::Result_YES) {
            safeMode = true;
        }
    }

    if (!safeMode) {
        // Load plugins
        NativeUI::SplashScreen::SetLoadingText(splashScreen, "Loading plugins...");
        Marmalade::PluginLoader::GetInstance().LoadPlugins();
    }

    if (project != nullptr) {
        // Open specified project
        NativeUI::SplashScreen::SetLoadingText(splashScreen, "Opening project...");
        if (application.OpenProject(project)) {
            Marmalade::GUI::WindowManager::GetInstance().welcomeScreen.visible = false;
        }
    }

    splashScreen.Close();
    while (!glfwWindowShouldClose(application.getWindow())) {
        application.Run();
    }

    application.OnClose();

    Marmalade::PluginLoader::GetInstance().UnloadPlugins();
    application.Terminate();

#if defined(__linux__)
    // Wait for GTK thread
    gtkThread.join();
#endif
    return 0;
}
