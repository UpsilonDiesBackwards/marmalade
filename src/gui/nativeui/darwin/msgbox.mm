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

#include "../msgbox.h"

using namespace Marmalade::GUI::NativeUI;

using Buttons = MsgBox::Buttons;
using Result = MsgBox::Result;

Result MsgBox::ShowMessage(const std::u16string& message, const std::u16string& title, MsgBox::Style style, Buttons buttons) {
    NSAlertStyle s;
    switch (style) {
        case Style::Style_INFO:
            s = NSAlertStyleInformational;
            break;
        case Style::Style_WARNING:
            s = NSAlertStyleWarning;
            break;
        case Style::Style_ERROR:
            s = NSAlertStyleCritical;
            break;
        case Style::Style_QUESTION:
            s = NSAlertStyleInformational;
            break;
    }

    NSAlert* alert = [[NSAlert alloc] init];

    [alert setMessageText:Util::utf16ToPlatformStr(title)];
    [alert setInformativeText:Util::utf16ToPlatformStr(message)];
    [alert setAlertStyle:s];

    switch (buttons) {
        case Buttons::Buttons_OK:
            [alert addButtonWithTitle:@"OK"];
            break;
        case Buttons::Buttons_OK_CANCEL:
            [alert addButtonWithTitle:@"OK"];
            [alert addButtonWithTitle:@"Cancel"];
            break;
        case Buttons::Buttons_YES_NO:
            [alert addButtonWithTitle:@"Yes"];
            [alert addButtonWithTitle:@"No"];
            break;
        default:
            [alert addButtonWithTitle:@"OK"];
    }

    NSModalResponse sel = [alert runModal];

    switch (buttons) {
        case Buttons::Buttons_OK:
            return sel == NSAlertFirstButtonReturn ? Result::Result_OK : Result::Result_NONE;
        case Buttons::Buttons_OK_CANCEL:
            if (sel == NSAlertFirstButtonReturn) {
                return Result::Result_OK;
            }
            if (sel == NSAlertSecondButtonReturn) {
                return Result::Result_CANCEL;
            }
            return Result::Result_NONE;
        case Buttons::Buttons_YES_NO:
            if (sel == NSAlertFirstButtonReturn) {
                return Result::Result_YES;
            }
            if (sel == NSAlertSecondButtonReturn) {
                return Result::Result_NO;
            }
            return Result::Result_NONE;
    }
}

Result MsgBox::ShowMessage(const Window& window, const std::u16string& message, const std::u16string& title, MsgBox::Style style, Buttons buttons) {
    return ShowMessage(message, title, style, buttons);
}
