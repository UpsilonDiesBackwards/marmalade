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

#include <imgui.h>

#include <IconsCodicons.h>

#include <nlohmann/json.hpp>

#include <fstream>

std::vector<Marmalade::GUI::About::Package> Marmalade::GUI::About::loadPackages(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to read licenses");
    }

    nlohmann::json package_json;
    file >> package_json;

    std::vector<Package> packages;
    for (const auto& pkg: package_json["packages"]) {
        packages.push_back({pkg["name"], pkg["license"]});
    }

    return packages;
}

void Marmalade::GUI::About::Draw() {
    ImGui::Begin(ICON_CI_INFO " About", &visible);

    static int aboutTexId = Util::LoadGuiTexture("res/icons/logo/logo1920.png");

    if (ImGui::BeginTabBar("AboutTabs")) {
        if (ImGui::BeginTabItem("About")) {
            ImGui::PushFont(FontManager::GetInstance().fontTitle);
            ImGui::Text("Marmalade Engine");
            ImGui::PopFont();

            ImGui::Text("A lightweight 2D game engine");

            ImGui::Image(aboutTexId, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Copyright (C) 2025");

            ImGui::PushFont(FontManager::GetInstance().fontSubheading);
            ImGui::Text("Contributors:");
            ImGui::PopFont();

            ImGui::Text("Tayler Parsons");
            ImGui::Text("Ryan Bester");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("3rd Party Licenses")) {
            ImGui::Text("Marmalade Engine would not be possible without the following libraries:");

            if (ImGui::BeginTabBar("AboutPackagesTabs")) {
                try {
                    static std::vector<Package> packages = loadPackages("res/packages.json");
                    for (const auto& pkg: packages) {
                        if (ImGui::BeginTabItem(pkg.name.c_str())) {
                            ImGui::Text("%s", pkg.license.c_str());

                            ImGui::EndTabItem();
                        }
                    }
                } catch (const std::exception& ex) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", ex.what());
                }
                ImGui::EndTabBar();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
