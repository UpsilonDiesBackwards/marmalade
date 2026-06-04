// Marmalade - Lightweight Game Engine
// Copyright (C) 2026 Tayler Parsons
// Copyright (C) 2026 Ryan Bester
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MARMALADE_PROJECT_SETTINGSCONTAINER_H
#define MARMALADE_PROJECT_SETTINGSCONTAINER_H

#include "projectassets.h"
#include "projectpackages.h"
#include "projectsettings.h"
#include "projectuser.h"

namespace Marmalade::Project {
    struct ProjectSettings;
    template<typename T>
    struct SettingsObject {
        std::string name;
        Config<T> config;

        T* operator->() noexcept {
            return &config.storedConfig;
        }
    };

    class SettingsContainer {
    public:
        SettingsObject<ProjectAssets> assets;
        SettingsObject<ProjectPackages> packages;
        SettingsObject<ProjectSettings> settings;
        SettingsObject<ProjectUser> user;
    };
}


#endif
