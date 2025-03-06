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

#ifndef MARMALADE_GUI_SETTINGS_H
#define MARMALADE_GUI_SETTINGS_H

#include "../window.h"

#include <string>
#include <unordered_map>
#include <functional>

namespace Marmalade::GUI {
    class Preferences : public Window {
        struct PreferencesPane {
            std::function<void()> DrawFunc{};
            std::function<void()> SaveFunc{};

            PreferencesPane() {};

            explicit PreferencesPane(const std::function<void()>& drawFunc) : DrawFunc(drawFunc) {}
        };

    public:
        explicit Preferences();

        void Draw() override;

    private:
        std::string _selectedItem{};

        std::unordered_map<std::string, PreferencesPane> _panes{};

        void selectableTreeNode(const char* title, const char* id);
        void drawLeftPane();
        void drawRightPane();
        void drawSplit();

        static void drawGeneralLoggingPane();
        static void drawGeneralAppearancePane();
        static void drawGeneralProjectsPane();

        static void requiresRestartWarning();
        static bool getLogLevels(void* data, int idx, const char** outText);
    };
}

#endif
