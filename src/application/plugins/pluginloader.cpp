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

#include "interfaceimpl.h"

#include "../config/configutil.h"
#include "../logger.h"

#include <m3_api_libc.h>
#include <wasm3.h>
#include <m3_env.h>

EngineAPI *gapi = nullptr;

int Marmalade::EngineApiImpl::GetVersion() {
    return 1;
}

Marmalade::PluginLoader& Marmalade::PluginLoader::GetInstance() {
    static PluginLoader instance{};
    return instance;
}

m3ApiRawFunction(wasm_engine_Log)
{
    m3ApiGetArgMem(const char*, text);
    LOG_INFO("[PLUGIN] {}\n", text);
    m3ApiSuccess();
}

m3ApiRawFunction(wasm_engine_GetVersion)
{
    m3ApiReturnType (int32_t);

    int version = gapi->GetVersion();
    m3ApiReturn(version);
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

    // Static methods, function pointers set by plugins might persist across plugins?
    static auto interfaceApi = InterfaceApiImpl::Create();

    static EngineAPI api{};
    gapi = &api;
    api.GetVersion = &EngineApiImpl::GetVersion;
    api.InterfaceApi = &interfaceApi;

    for (const auto& pluginFile: std::filesystem::directory_iterator(pluginsDir)) {
        LOG_INFO("Loading plugin: {}", pluginFile.path().filename().string());

        if (pluginFile.path().extension() == ".wasm") {
            // Wasm loader
            M3Result result = nullptr;

            // FIXME: Environment should be global
            IM3Environment env = m3_NewEnvironment();
            if (!env) {
                LOG_ERROR("m3_NewEnvironment failed");
                continue;
            }

#define WASM_STACK_SIZE 64 * 1024
            IM3Runtime runtime = m3_NewRuntime(env, WASM_STACK_SIZE, nullptr);
            if (!runtime) {
                LOG_ERROR("m3_NewRuntime failed");
                continue;
            }

            FILE* file = fopen(pluginFile.path().string().c_str(), "rb");
            if (!file) {
                LOG_ERROR("Failed to open WASM file");
                continue;
            }
            fseek(file, 0, SEEK_END);
            size_t size = ftell(file);
            rewind(file);

            std::vector<uint8_t> buffer(size);
            fread(buffer.data(), 1, size, file);
            fclose(file);

            IM3Module module;
            result = m3_ParseModule(env, &module, buffer.data(), size);
            if (result) {
                LOG_ERROR("m3_ParseModule failed: {}", result);
                continue;
            }

            result = m3_LoadModule(runtime, module);
            if (result) {
                LOG_ERROR("m3_LoadModule failed: {}", result);
                continue;
            }

            m3_LinkLibC(module);

            result = m3_LinkRawFunction(module, "engine", "Log", "v(*)", wasm_engine_Log);
            if (result != m3Err_none) {
                LOG_ERROR("Failed to link engine_Log: {}", result);
            }

            result = m3_LinkRawFunction(module, "engine", "GetVersion", "i()", wasm_engine_GetVersion);
            if (result != m3Err_none) {
                LOG_ERROR("Failed to link engine_GetVersion: {}", result);
            }

            IM3Function function;
            result = m3_FindFunction(&function, runtime, "PluginMain");
            if (result) {
                LOG_ERROR("m3_FindFunction failed: {}", result);
                continue;
            }

            result = m3_CallV(function);
            if (result) {
                LOG_ERROR("m3_CallV failed: {}", result);
                continue;
            }

            Plugin plugin{};
            plugin.Path = pluginFile.path();
            plugin.Type = PluginType_WASM;
            _loadedPlugins.push_back(plugin);
        } else {
            // Native (.so, .dll, .dylib)
            auto pluginLib = loadPluginLibrary(pluginFile.path());
            if (pluginLib == nullptr) continue;

            Plugin plugin{pluginLib};

            // TODO: Use plugin name from manifest
            auto pluginName = pluginFile.path().filename().string();
            plugin.logger = createPluginLogger(pluginName);
            static auto pluginLogger = PluginLogger{
                .LogTrace = &PluginLoggerImpl::LogTrace,
                .LogDebug = &PluginLoggerImpl::LogDebug,
                .LogInfo = &PluginLoggerImpl::LogInfo,
                .LogWarn = &PluginLoggerImpl::LogWarn,
                .LogError = &PluginLoggerImpl::LogError,
                .LogCritical = &PluginLoggerImpl::LogCritical,
                ._logger = &plugin.logger};

            api.Logger = &pluginLogger;
            callPluginMain(plugin, api);
            plugin.Api = api;
            plugin.Path = pluginFile.path();
            plugin.Type = PluginType_NATIVE;
            _loadedPlugins.push_back(plugin);
        }
    }
}

void Marmalade::PluginLoader::UnloadPlugins() {
    LOG_INFO("Unloading plugins");
    for (auto& plugin: _loadedPlugins) {
        if (plugin.Type == PluginType_NATIVE) {
#ifdef _WIN32
            FreeLibrary(plugin.library);
#else
            dlclose(plugin.library);
#endif
        }
    }
}

std::vector<Marmalade::Plugin>& Marmalade::PluginLoader::GetLoadedPlugins() {
    return _loadedPlugins;
}

LIBRARY_TYPE Marmalade::PluginLoader::loadPluginLibrary(const std::filesystem::path& path) {
#ifdef _WIN32
    HMODULE pluginLib = LoadLibraryA(path.string().c_str());
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

std::shared_ptr<spdlog::logger> Marmalade::PluginLoader::createPluginLogger(std::string pluginName) {
    auto root = spdlog::get("EngineLogger");
    auto& sinks = root->sinks();
    auto logger = std::make_shared<spdlog::logger>(pluginName, sinks.begin(), sinks.end());
    logger->set_level(root->level());
    spdlog::register_logger(logger);
    return logger;
}
