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

#include "../app.h"

static void activate(GtkApplication *app, gpointer user_data) {
    auto* appObj = static_cast<Marmalade::GUI::NativeUI::App*>(user_data);
    if (appObj->GetCreateCallback() != nullptr) {
        appObj->GetCreateCallback()(app);
    }
}

void Marmalade::GUI::NativeUI::App::Create(int argc, char **argv) {
    GtkApplication *app;

    app = gtk_application_new("com.diesbackwards.marmalade", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), (void *)this);
    g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
}
