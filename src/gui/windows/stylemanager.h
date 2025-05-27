/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ENGINE_STYLEMANAGER_H
#define ENGINE_STYLEMANAGER_H

#include <string>
/*
* \brief Marmalade style manager. Sets and loads the interface style
*/
class StyleManager {
public:
    StyleManager();

    void LoadStyle(const std::string& stylePath);
    void SaveStyle(const std::string& stylePath);
private:
    std::string stylePath;
};

#define COL_CATPPUCCIN_UI_RED ImVec4(0.95f, 0.54f, 0.65f, 1.0f)
#define COL_CATPPUCCIN_UI_GREEN ImVec4(0.65f, 0.89f, 0.63f, 1.0f)
#define COL_CATPPUCCIN_UI_BLUE ImVec4(0.53f, 0.70f, 0.98f, 1.0f)


#endif
