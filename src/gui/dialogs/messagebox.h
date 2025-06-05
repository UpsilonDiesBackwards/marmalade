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

#ifndef MARMALADE_GUI_MESSAGEBOX_H
#define MARMALADE_GUI_MESSAGEBOX_H

#include "../dialog.h"

namespace Marmalade::GUI {
    class MsgBox final : public CustomDialog {
    public:
        enum Buttons {
            Buttons_OK,
            Buttons_OK_CANCEL,
            Buttons_YES_NO,
            Buttons_YES_NO_CANCEL
        };

        enum ResultType {
            ResultType_OK,
            ResultType_CANCEL,
            ResultType_YES,
            ResultType_NO
        };

        struct Result {
            ResultType Result;
        };

        void Draw() override;
        std::string GetName() override;

        MsgBox(const std::string& title, const std::string& message, const Buttons buttons) : _title(title), _message(message), _buttons(buttons) {}

        static void ShowMsgBox(const std::string& title, const std::string& message, Buttons buttons = Buttons_OK, std::function<void(Result*)> callback = nullptr);

    private:
        std::string _title;
        std::string _message;
        Buttons _buttons;
    };
}

#endif
