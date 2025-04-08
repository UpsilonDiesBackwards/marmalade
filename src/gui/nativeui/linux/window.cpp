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

#include <gtk-3.0/gtk/gtk.h>

#include "../window.h"
#include "../msgbox.h"
#include "../util.h"

bool Marmalade::GUI::NativeUI::Window::Create() {
    if (nullptr == this->_app) {
        return false;
    }

    _handle = gtk_application_window_new(this->_app);
    auto title = Marmalade::GUI::NativeUI::Util::utf16ToPlatformStr(this->_title);
    gtk_window_set_title(GTK_WINDOW(_handle), title);
    gtk_window_set_default_size(GTK_WINDOW(_handle), this->_width, this->_height);

    g_free(title);

    return true;
}

bool Marmalade::GUI::NativeUI::Window::Show(bool topmost) {
    gtk_widget_show_all(_handle);
    return true;
}

void Marmalade::GUI::NativeUI::Window::SetApp(GtkApplication* app) {
    this->_app = app;
}

Marmalade::GUI::NativeUI::Window Marmalade::GUI::NativeUI::Window::WrapGlfwWindow(GLFWwindow* window) {
    return {nullptr};
}

void Marmalade::GUI::NativeUI::Window::SetCreateCallback(std::function<void()> createCallback) {
    _create_callback = createCallback;
}

void Marmalade::GUI::NativeUI::Window::Close() {
    gtk_window_close(GTK_WINDOW(_handle));
}
