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

#ifndef MARMALADE_ENGINEAPI_H
#define MARMALADE_ENGINEAPI_H

#ifdef _MSC_VER
#define ENGINE_PROVIDED
#define PLUGIN_PROVIDED
#else
#define ENGINE_PROVIDED __attribute__((annotate("engine_provided")))
#define PLUGIN_PROVIDED __attribute__((annotate("plugin_provided")))
#endif

typedef enum {
    MARM_RESULT_SUCCESS = 0,
    MARM_RESULT_FAILURE = 1,
} MarmResult;

struct ENGINE_PROVIDED PluginLogger {
    void (*LogTrace)(void* logger, const char* fmt, ...);
    void (*LogDebug)(void* logger, const char* fmt, ...);
    void (*LogInfo)(void* logger, const char* fmt, ...);
    void (*LogWarn)(void* logger, const char* fmt, ...);
    void (*LogError)(void* logger, const char* fmt, ...);
    void (*LogCritical)(void* logger, const char* fmt, ...);
    void* const _logger;
};

#ifndef ENGINE_BUILD
#define LOG_TRACE(engineApi, fmt, ...) (engineApi)->Logger->LogTrace((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#define LOG_DEBUG(engineApi, fmt, ...) (engineApi)->Logger->LogDebug((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#define LOG_INFO(engineApi, fmt, ...) (engineApi)->Logger->LogInfo((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#define LOG_WARN(engineApi, fmt, ...) (engineApi)->Logger->LogWarn((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#define LOG_ERROR(engineApi, fmt, ...) (engineApi)->Logger->LogError((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#define LOG_CRITICAL(engineApi, fmt, ...) (engineApi)->Logger->LogCritical((engineApi)->Logger->_logger, fmt, __VA_ARGS__)
#endif

// Each structure contains a Size field to allow for ABI safety
#define SAFE_TO_CALL(type, api, func) (sizeof(*(api)) >= (offsetof(type, func) + sizeof((api)->func)))

struct InterfaceAPI;

struct ENGINE_PROVIDED EngineAPI {
    int (*GetVersion)();
    struct PluginLogger* Logger;

    // Structures for each API, in alphabetical order
    struct InterfaceAPI* InterfaceApi;
};

#ifdef WASM_IMPORT

WASM_IMPORT(engine, Log) extern void engine_Log(const char* text);
WASM_IMPORT(engine, GetVersion) extern int engine_GetVersion();

#endif


#endif
