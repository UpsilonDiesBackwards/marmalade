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

#ifndef MARMALADE_GUI_WINDOWMANAGER_H
#define MARMALADE_GUI_WINDOWMANAGER_H

#include "../application/config/engineconfig.h"
#include "window.h"
#include "dialog.h"

#include "windows/welcomescreen.h"
#include "windows/packagemanager.h"
#include "windows/animation.h"
#include "windows/versioncontrol.h"

#include "log/log.h"

#include "wizards/projectwizard.h"

#include "settings/preferences.h"
#include "settings/settings.h"

#include "editor/projectbrowser.h"

#include "dialogs/about.h"
#include "dialogs/configerror.h"
#include "dialogs/saveworkspace.h"
#include "windows/animation.h"
#include "windows/versioncontrol.h"

#include <vector>

/// \cond FORWARD_DECL_HIDE
// Forward decl
namespace IGFD {
    struct FileDialogConfig;
}
typedef int ImGuiFileDialogFlags;
const int FDF_Modal = 1 << 9;
/// \endcond

namespace Marmalade::GUI {
    /**
     * \brief Manages visibility of windows and dialogs.
     *
     * Windows are stored in their own field, and are added to the \ref windows vector.
     *
     * Dialogs and file dialogs are registered with the `RegisterDialog()` and `RegisterFileDialog()` methods.
     *
     * See \ref cp-windows-and-dialogs for more information.
     */
    class WindowManager {
    public:
        struct FileDialogResult {
            std::string FilePath;
        };

        static WindowManager& GetInstance();

        Marmalade::GUI::WelcomeScreen welcomeScreen{Marmalade::EngineConfig::GetStoredConfig().appearance.showWelcomeScreen};

        Animation animationManager{};
        PackageManager packageManager{};
        Log log{true};
        ProjectWizard projectWizard{};
        Preferences preferences{};
        ProjectSettings settings{};
        ProjectBrowser projectBrowser{true};
        VersionControl versionControl{};
        About about{};
        ConfigErrorDialog configErrorDlg{};

        std::vector<Marmalade::GUI::Window*> windows{};
        std::vector<Dialog> dialogs{};
        std::vector<Dialog> fileDialogs{};

        bool showStyleEditor = false;
        bool showDebugWindow = false;
#if DEBUG
        bool showImGuiTestsWindow = false;
#endif

        void ToggleDebugWindow();

#if DEBUG
        void ToggleImGuiTestsWindow();
#endif

        /**
         * \brief Registers a custom dialog.
         * \param customDlg Instance of an object extending `CustomDialog`.
         * \param callback The callback function.
         * \param reregister Whether the object should be reassigned.
         * \param flags ImGui window flags.
         * \param minSize The minimum size of the dialog.
         * \return The `Dialog` object.
         */
        Dialog RegisterDialog(std::shared_ptr<CustomDialog> customDlg, const DialogCallback& callback, bool reregister = false, ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse, ImVec2 minSize = ImVec2(800, 500));

        /**
         * \brief Shows a custom dialog.
         * \param name The name of the dialog.
         */
        void ShowDialog(std::string name);

        /**
         * \brief Prepares the config for a file dialog.
         * \param path The default path. If empty, set to the current project directory.
         * \param flags File dialog flags.
         * \return The `FileDialogConfig` object.
         */
        static IGFD::FileDialogConfig PrepareFileDialogConfig(const std::string& path = "", int flags = FDF_Modal);

        /**
         * \brief Registers a file dialog.
         * \param name The name of the dialog.
         * \param callback The callback function.
         * \param flags ImGui window flags.
         * \param minSize The minimum size of the dialog.
         * \return The `Dialog` object.
         */
        Dialog RegisterFileDialog(std::string name, const DialogCallback& callback, ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse, ImVec2 minSize = ImVec2(800, 500));

    private:
        WindowManager();
    };
}


#endif
