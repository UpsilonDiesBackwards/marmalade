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

#ifndef MARMALADE_INSTALLER_COMPONENTS_H
#define MARMALADE_INSTALLER_COMPONENTS_H

#include <string>
#include <vector>
#include <sstream>

struct Component {
    std::wstring DisplayName;
    std::string DirName;
    bool Required;
    bool Selected;
};

std::wstring ComponentsToParams(const std::vector<Component>& components) {
    std::wstringstream ss;
    bool first = true;

    for (size_t i = 0; i < components.size(); ++i) {
        if (components[i].Selected) {
            if (!first) ss << L",";
            ss << i;
            first = false;
        }
    }

    return ss.str();
}

void ComponentsFromParams(const std::wstring& params, std::vector<Component>& components) {
    std::wstringstream ss(params);
    std::wstring token;

    for (auto& c: components) {
        if (!c.Required) {
            c.Selected = false;
        }
    }

    while (std::getline(ss, token, L',')) {
        try {
            size_t index = std::stoul(token);
            if (index < components.size()) {
                components[index].Selected = true;
            }
        } catch (const std::exception& ex) {
            // Ignore
        }
    }
}

#endif
