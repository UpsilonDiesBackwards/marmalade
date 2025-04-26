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

#include "interfaceimpl.h"

#include "../logger.h"

#include <regex>

using namespace Marmalade;

std::string InterfaceApiImpl::_focusedWindowName = "";
std::unordered_map<std::string, bool> InterfaceApiImpl::_hookCacheStatus{};
std::unordered_map<std::string, std::vector<std::function<void()>>> InterfaceApiImpl::_beginHooksCached{};
std::unordered_map<std::string, std::vector<std::function<void()>>> InterfaceApiImpl::_endHooksCached{};
std::unordered_map<std::string, std::vector<std::function<void()>>> InterfaceApiImpl::_beginHooks{};
std::unordered_map<std::string, std::vector<std::function<void()>>> InterfaceApiImpl::_endHooks{};

void InterfaceApiImpl::AddBeginHook(const char* nameRegex, void (*BeginHook)()) {
    _beginHooks[nameRegex].emplace_back(BeginHook);
}

void InterfaceApiImpl::AddEndHook(const char* nameRegex, void (*EndHook)()) {
    _endHooks[nameRegex].emplace_back(EndHook);
}

const char* InterfaceApiImpl::GetFocusedWindow() {
    auto str= _focusedWindowName.c_str();
    return str;
}

void InterfaceApiImpl::CallBeginHooks(std::string windowName) {
    if (!inCache(windowName)) {
        cacheHooks(windowName);
    }

    for (auto& hook: _beginHooksCached[windowName]) {
        hook();
    }
}

void InterfaceApiImpl::CallEndHooks(std::string windowName) {
    if (!inCache(windowName)) {
        cacheHooks(windowName);
    }

    for (auto& hook: _endHooksCached[windowName]) {
        hook();
    }
}

void InterfaceApiImpl::SetFocusedWindow(std::string windowName) {
    // Remove unicode stuff
    std::string result;
    std::copy_if(windowName.begin(), windowName.end(), std::back_inserter(result),
                 [](unsigned char c) { return c <= 0x7F && c != 0x00; });
    _focusedWindowName = result;
}

void InterfaceApiImpl::cacheHooks(std::string windowName) {
    for (const auto& [pattern, hooks]: _beginHooks) {
        try {
            std::regex re(pattern);
            if (std::regex_match(windowName, re)) {
                _beginHooksCached[windowName] = hooks;
            }
        } catch (const std::regex_error& e) {
            LOG_ERROR("Invalid regex: {}", e.what());
        }
    }

    for (const auto& [pattern, hooks]: _endHooks) {
        try {
            std::regex re(pattern);
            if (std::regex_match(windowName, re)) {
                _endHooksCached[windowName] = hooks;
            }
        } catch (const std::regex_error& e) {
            LOG_ERROR("Invalid regex: {}", e.what());
        }
    }

    _hookCacheStatus[windowName] = true;
}

bool InterfaceApiImpl::inCache(std::string windowName) {
    return _hookCacheStatus.contains(windowName) && _hookCacheStatus[windowName] == true;
}
