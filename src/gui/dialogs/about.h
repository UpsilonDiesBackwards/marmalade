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

#ifndef MARMALADE_GUI_ABOUT_H
#define MARMALADE_GUI_ABOUT_H

#include "../window.h"
#include "../components/splitter.h"

#include <map>
#include <string>

namespace Marmalade::GUI {
    class AboutLicensesSplitter : public Components::Splitter {
    public:
        struct Package {
            std::string name;
            std::string license;
        };

        explicit AboutLicensesSplitter() : Splitter("AboutLicenses") {}

        void DrawLeftPane() override;
        void DrawRightPane() override;

    private:
        std::map<std::string, Package> _packages{};
        std::string _selectedItem{};

        std::map<std::string, Package> loadPackages(const std::string& filename);
    };

    /**
     * \brief Code for the 'About' GUI window
     * \brief Shows the engine information, license information for packages and contributor names 
     */
    class About : public Window {
    public:
        void Draw() override;

    private:
        AboutLicensesSplitter _splitter{};

    };
}


#endif
