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

#include <plugin.h>

#ifdef _WIN32
#include <Windows.h>

#define LIBRARY_TYPE HMODULE
#else
#include <dlfcn.h>

#define LIBRARY_TYPE void*
#endif

#include <spdlog/logger.h>

#include <stdarg.h>
#include <vector>
#include <filesystem>

#define PLUGIN_LOG_FUNC(level)                                                        \
    auto* spdlog_logger = reinterpret_cast<std::shared_ptr<spdlog::logger>*>(logger); \
    char buffer[1024];                                                                \
    va_list args;                                                                     \
    va_start(args, fmt);                                                              \
    vsnprintf(buffer, sizeof(buffer), fmt, args);                                     \
    va_end(args);                                                                     \
    spdlog_logger->get()->level("{}", buffer);


namespace Marmalade {
    class Plugin {
    public:
        LIBRARY_TYPE library;
        std::shared_ptr<spdlog::logger> logger;
        EngineAPI Api;
    };

    class PluginLoader {
    public:
        static PluginLoader& GetInstance();

        void LoadPlugins();
        void UnloadPlugins();
    private:
        std::vector<Plugin> _loadedPlugins{};

        LIBRARY_TYPE loadPluginLibrary(const std::filesystem::path& path);
        void callPluginMain(Plugin plugin, EngineAPI engineApi);

        std::shared_ptr<spdlog::logger> createPluginLogger(std::string pluginName);
    };

    class EngineApiImpl {
    public:
        static int GetVersion();
    };

    class PluginLoggerImpl {
    public:
        static void LogTrace(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(trace); }
        static void LogDebug(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(debug); }
        static void LogInfo(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(info); }
        static void LogWarn(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(warn); }
        static void LogError(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(error); }
        static void LogCritical(void* logger, const char* fmt, ...) { PLUGIN_LOG_FUNC(critical); }
    };
}

#endif
