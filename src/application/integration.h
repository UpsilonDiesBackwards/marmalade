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

#ifndef MARMALADE_APPLICATION_INTEGRATION_H
#define MARMALADE_APPLICATION_INTEGRATION_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <filesystem>

namespace Marmalade::Application {
    class Integration {
    public:
        enum IntegrationType {
            IntegrationType_NONE = 0,
            IntegrationType_FILE_ASSOCIATION = (1u << 0),
            IntegrationType_LAUNCHER = (1u << 1)
        };

#ifdef WIN32
        static bool SetRegistryValue(HKEY root, const std::wstring& subkey, const std::wstring& valName, const std::wstring& val);
#endif

        static void AddSystemIntegrations(IntegrationType types);

        static void MarkRecentFile(const std::filesystem::path &path);
    };
}


#endif
