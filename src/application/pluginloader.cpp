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

#include "pluginloader.h"

#include "config/configutil.h"
#include "logger.h"

int Marmalade::EngineApiImpl::GetVersion() {
    return 1;
}

void Marmalade::EngineApiImpl::Print(char* str) {
    LOG_INFO("From plugin: {}", str);
}

Marmalade::PluginLoader Marmalade::PluginLoader::GetInstance() {
    static PluginLoader instance = PluginLoader();
    return instance;
}

void Marmalade::PluginLoader::LoadPlugins() {
    // TODO: Plugins need to be cross-platform.
    // TODO: e.g. compiled as PE .dll for Windows and ELF .so for Linux
    // TODO: Loader will choose what file to load depending on manifest (manifest needs to be loaded first)

    auto configDir = ConfigUtil::GetConfigDirectory();
    auto pluginsDir = configDir / "plugins";

    if (!std::filesystem::exists(pluginsDir)) {
        std::filesystem::create_directories(pluginsDir);
    }

    _loadedPlugins.clear();

    EngineAPI api{};
    api.GetVersion = &EngineApiImpl::GetVersion;

    for (const auto& pluginFile: std::filesystem::directory_iterator(pluginsDir)) {
        LOG_INFO("Loading plugin: {}", pluginFile.path().filename().string());

        auto pluginLib = loadPluginLibrary(pluginFile.path());
        if (pluginLib == nullptr) continue;

        Plugin plugin{pluginLib};
        callPluginMain(plugin, api);

        _loadedPlugins.push_back(plugin);
    }
}

void Marmalade::PluginLoader::UnloadPlugins() {
    LOG_INFO("Unloading plugins");
    for (auto& plugin: _loadedPlugins) {
#ifdef _WIN32
        FreeLibrary(plugin.library);
#else
        dlclose(plugin.library);
#endif
    }
}

LIBRARY_TYPE Marmalade::PluginLoader::loadPluginLibrary(const std::filesystem::path& path) {
#ifdef _WIN32
    HMODULE pluginLib = LoadLibrary(path.string().c_str());
    if (!pluginLib) {
        LOG_ERROR("Failed to load plugin: {}", path.filename().string());
        return nullptr;
    }

    return pluginLib;
#else
    void* pluginLib = dlopen(path.string().c_str(), RTLD_LAZY);
    if (!pluginLib) {
        LOG_ERROR("Failed to load plugin: {}", path.filename().string());
        return nullptr;
    }

    return pluginLib;
#endif
}

void Marmalade::PluginLoader::callPluginMain(Plugin plugin, EngineAPI engineApi) {
#ifdef _WIN32
    auto pluginMain = (PluginMainFunc) GetProcAddress(plugin.library, "PluginMain");
    if (!pluginMain) {
        LOG_ERROR("Failed to find function: PluginMain");
        FreeLibrary(plugin.library);
        return;
    }
#else
    auto pluginMain = (PluginMainFunc) dlsym(plugin.library, "PluginMain");
    const char* error = dlerror();
    if (error) {
        LOG_ERROR("Failed to find function: PluginMain");
        dlclose(plugin.library);
        return;
    }
#endif

    pluginMain(engineApi);
}
