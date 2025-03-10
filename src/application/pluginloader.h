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

#ifndef MARMALADE_PLUGINLOADER_H
#define MARMALADE_PLUGINLOADER_H

#include "plugin.h"

#ifdef _WIN32
#include <Windows.h>

#define LIBRARY_TYPE HMODULE
#else
#include <dlfcn.h>

#define LIBRARY_TYPE void*
#endif

#include <vector>
#include <filesystem>

namespace Marmalade {
    class Plugin {
    public:
        LIBRARY_TYPE library;
    };

    class PluginLoader {
    public:
        static PluginLoader GetInstance();

        void LoadPlugins();
        void UnloadPlugins();
    private:
        std::vector<Plugin> _loadedPlugins{};

        LIBRARY_TYPE loadPluginLibrary(const std::filesystem::path& path);
        void callPluginMain(Plugin plugin, EngineAPI engineApi);
    };

    class EngineApiImpl {
    public:
        static int GetVersion();
    };
}


#endif
