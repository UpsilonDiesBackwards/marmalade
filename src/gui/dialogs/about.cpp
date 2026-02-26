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

#include "about.h"

#include "../../application/util.h"
#include "../fontmanager.h"
#include "../../application/application.h"
#include "../../application/plugins/interfaceimpl.h"
#include "../../application/i18n.h"

#include <imgui.h>

#include <IconsCodicons.h>

#include <nlohmann/json.hpp>

#include <fstream>


void Marmalade::GUI::AboutLicensesSplitter::DrawLeftPane() {
    try {
        _packages = loadPackages("res/packages.json");
        for (const auto& key: _packages | std::views::keys) {
            if (ImGui::Selectable(key.c_str(), _selectedItem == key)) {
                _selectedItem = key;
            }
        }
    } catch (const std::exception& ex) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", ex.what());
    }
}

void Marmalade::GUI::AboutLicensesSplitter::DrawRightPane() {
    if (_selectedItem.empty()) {
        ImGui::Text("Select an item on the left");
    } else {
        if (const auto pkg = _packages.find(_selectedItem); pkg != _packages.end()) {
            ImGui::Text("%s", pkg->second.license.c_str());
        } else {
            ImGui::Text("License text does not exist");
        }
    }
}

/**
 * \brief Loads the information about packages included in the current project
 * \param filename File path of the packages.json file
 * \return std::map<std::string, Marmalade::GUI::AboutLicensesSplitter::Package> Map of packages included in the current project
 */
std::map<std::string, Marmalade::GUI::AboutLicensesSplitter::Package> Marmalade::GUI::AboutLicensesSplitter::loadPackages(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to read licenses");
    }

    nlohmann::json package_json;
    file >> package_json;

    std::map<std::string, Package> packages;
    for (const auto& pkg : package_json["packages"]) {
        packages.insert({
            pkg["name"].get<std::string>(),
            { pkg["name"].get<std::string>(), pkg["license"].get<std::string>() }
        });
    }

    return packages;
}

/**
 * \brief Draws the 'About' GUI window. Shows the (temp) Marmalade logo and creates a list of tabs for each package and 3rd Party license
 */
void Marmalade::GUI::About::Draw() {
    WINDOW_BEGIN(ICON_CI_INFO " About", ImGuiWindowFlags_None)

    static int aboutTexId = Util::LoadGuiTexture("res/icons/logo/logo1920.png");

    if (ImGui::BeginTabBar("AboutTabs")) {
        if (ImGui::BeginTabItem("About")) {
            ImGui::PushFont(nullptr, FontManager::GetInstance().titleSize);
            ImGui::Text("Marmalade Engine");
            ImGui::PopFont();

            ImGui::Text(_("A lightweight 2D game engine"));

            ImGui::Image(aboutTexId, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Copyright (C) 2025");

            ImGui::PushFont(nullptr, FontManager::GetInstance().subheadingSize);
            ImGui::Text("Contributors:");
            ImGui::PopFont();

            ImGui::Text("Tayler Parsons");
            ImGui::Text("Ryan Bester");

            ImGui::Text("Commit: %s", Application::GetInstance().COMMIT);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("3rd Party Licenses")) {
            ImGui::Text("Marmalade Engine would not be possible without the following libraries:");

            _splitter.Draw();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    WINDOW_END()
}
