
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

#ifndef MARMALADE_AUTOSAVE_H
#define MARMALADE_AUTOSAVE_H

#include <string>
#include <chrono>

#include "autosave.h"
#include "i18n.h"

#include "../gui/dialogs/autosave.h"

class AutoSave {
public:
    std::string lastSaveTimeStamp = _("Not yet");

    std::chrono::steady_clock::time_point toastStartTime;
    static constexpr std::chrono::seconds toastDuration{5};

    void SaveEngine();
private:
    void ShowToast();

    std::string GetCurrentTimeStamp();
};


#endif//MARMALADE_AUTOSAVE_H
