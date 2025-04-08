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
#include "../util.h"

#include <gtk-3.0/gtk/gtk.h>

using Style = Marmalade::GUI::NativeUI::MsgBox::Style;
using Buttons = Marmalade::GUI::NativeUI::MsgBox::Buttons;
using Result = Marmalade::GUI::NativeUI::MsgBox::Result;

Result linuxCreateDialog(GtkWidget* wnd, const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    if (!gtk_init_check(nullptr, nullptr)) {
        return Result::Result_NONE;
    }

    GtkMessageType type;
    GtkButtonsType btn;

    switch (style) {
        case Style::Style_INFO:
            type = GTK_MESSAGE_INFO;
            break;
        case Style::Style_WARNING:
            type = GTK_MESSAGE_WARNING;
            break;
        case Style::Style_ERROR:
            type = GTK_MESSAGE_ERROR;
            break;
        case Style::Style_QUESTION:
            type = GTK_MESSAGE_QUESTION;
            break;
    }

    switch (buttons) {
        case Buttons::Buttons_OK:
            btn = GTK_BUTTONS_OK;
            break;
        case Buttons::Buttons_OK_CANCEL:
            btn = GTK_BUTTONS_OK_CANCEL;
            break;
        case Buttons::Buttons_YES_NO:
            btn = GTK_BUTTONS_YES_NO;
            break;
    }

    GtkWidget* dialog = gtk_message_dialog_new(
            GTK_WINDOW(wnd),
            GTK_DIALOG_MODAL,
            type,
            btn,
            "%s",
            Marmalade::GUI::NativeUI::Util::utf16ToPlatformStr(message));
    gtk_window_set_title(GTK_WINDOW(dialog), Marmalade::GUI::NativeUI::Util::utf16ToPlatformStr(title));

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(GTK_WIDGET(dialog));
    while (g_main_context_iteration(nullptr, false));

    switch (result) {
        case GTK_RESPONSE_OK:
            return Result::Result_OK;
        case GTK_RESPONSE_CANCEL:
            return Result::Result_CANCEL;
        case GTK_RESPONSE_YES:
            return Result::Result_YES;
        case GTK_RESPONSE_NO:
            return Result::Result_NO;
        default:
            return Result::Result_NONE;
    }
}

Result Marmalade::GUI::NativeUI::MsgBox::ShowMessage(const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    return linuxCreateDialog(nullptr, message, title, style, buttons);
}

Result Marmalade::GUI::NativeUI::MsgBox::ShowMessage(const Marmalade::GUI::NativeUI::Window& window, const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    return linuxCreateDialog(window.GetHandle(), message, title, style, buttons);
}
