// Copyright 2024 Ryan Bester, Tayler Parsons
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../msgbox.h"
#include "../util.h"

#include <gtk-3.0/gtk/gtk.h>

using Style = Marmalade::GUI::NativeUI::MsgBox::Style;
using Buttons = Marmalade::GUI::NativeUI::MsgBox::Buttons;
using Result = Marmalade::GUI::NativeUI::MsgBox::Result;

Result linuxCreateDialog(GtkWidget* wnd, const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    if (!gtk_init_check(nullptr, nullptr)) {
        return Result::None;
    }

    GtkMessageType type;
    GtkButtonsType btn;

    switch (style) {
        case Style::Style_INFO:
            type = GTK_MESSAGE_INFO;
            break;
        case Style::Style_Warning:
            type = GTK_MESSAGE_WARNING;
            break;
        case Style::Style_Error:
            type = GTK_MESSAGE_ERROR;
            break;
        case Style::Style_Question:
            type = GTK_MESSAGE_QUESTION;
            break;
    }

    switch (buttons) {
        case Buttons::Style_OK:
            btn = GTK_BUTTONS_OK;
            break;
        case Buttons::Style_OKCancel:
            btn = GTK_BUTTONS_OK_CANCEL;
            break;
        case Buttons::Style_YesNo:
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
            return Result::Result_Cancel;
        case GTK_RESPONSE_YES:
            return Result::Result_Yes;
        case GTK_RESPONSE_NO:
            return Result::Result_No;
        default:
            return Result::Result_None;
    }
}

Result Marmalade::GUI::NativeUI::MsgBox::show_message(const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    return linuxCreateDialog(nullptr, message, title, style, buttons);
}

Result Marmalade::GUI::NativeUI::MsgBox::show_message(const Marmalade::GUI::NativeUI::Window& window, const std::u16string& message, const std::u16string& title, Style style, Buttons buttons) {
    return linuxCreateDialog(window.get_handle(), message, title, style, buttons);
}
