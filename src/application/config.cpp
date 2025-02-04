#include "config.h"

#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#define APP_NAME "marmalade-engine"

std::filesystem::path Marmalade::Config::_configDir{};

void Marmalade::Config::SetConfigDirectory(bool sameDirConfig) {
    if (sameDirConfig) {
        _configDir = std::filesystem::current_path();
        return;
    }

#ifdef _WIN32
    char* appData = std::getenv("APPDATA");
    if (appData) {
        _configDir = std::filesystem::path(appData) / APP_NAME;
    } else {
        _configDir = std::filesystem::path("C:/Users/Default/AppData/Roaming") / APP_NAME;
    }
#elif __APPLE__
    const char* home = std::getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    _configDir = std::filesystem::path(home) / "Library" / "Application Support" / APP_NAME;
#else
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfig) {
        _configDir = std::filesystem::path(xdgConfig) / APP_NAME;
    } else {
        const char* home = std::getenv("HOME");
        if (!home) home = getpwuid(getuid())->pw_dir;
        _configDir = std::filesystem::path(home) / ".config" / APP_NAME;
    }
#endif

    if (!std::filesystem::exists(_configDir)) {
        std::filesystem::create_directories(_configDir);
    }
}

std::filesystem::path Marmalade::Config::GetConfigDirectory() {
    return _configDir;
}
