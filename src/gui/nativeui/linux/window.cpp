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

bool Marmalade::GUI::NativeUI::Window::Show() {
    gtk_widget_show_all(_handle);
    return true;
}

void Marmalade::GUI::NativeUI::Window::set_app(GtkApplication* app) {
    this->_app = app;
}
