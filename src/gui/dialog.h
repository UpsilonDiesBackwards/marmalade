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

#ifndef MARMALADE_GUI_CUSTOMDIALOG_H
#define MARMALADE_GUI_CUSTOMDIALOG_H

#include "window.h"

#include <imgui.h>

#include <memory>
#include <string>
#include <functional>

namespace Marmalade::GUI {
    struct Dialog;

    /**
     * \brief Callback on dialog result.
     *
     * \param result Whether the dialog was confirmed (true) or cancelled (false).
     * \param data A user-provided pointer passed to the callback.
     */
    using DialogCallback = std::function<void(bool result, void* data)>;

    class CustomDialog : public Window {
    public:
        virtual std::string GetName() = 0;

        void SetDialog(Dialog* dialog) {
            _dialog = dialog;
        }

    protected:
        Dialog* _dialog;
    };

    struct Dialog {
        std::shared_ptr<CustomDialog> CustomDlg;
        std::string Name;
        ImGuiWindowFlags Flags;
        ImVec2 MinSize;
        DialogCallback Callback;
    };

    class DialogButtons {
    public:
        static void AddButton(const char* primaryLbl, const std::function<void()>& primaryFunc);
        static void AddTwoButtons(const char* primaryLbl, const std::function<void()>& primaryFunc, const char* secondaryLbl, const std::function<void()>& secondaryFunc);
        static void AddThreeButtons(const char* primaryLbl, const std::function<void()>& primaryFunc, const char* secondaryLbl, const std::function<void()>& secondaryFunc, const char* ternaryLbl, const std::function<void()>& ternaryFunc);

        static void Ok(const std::function<void()>& okFunc);
        static void OkCancel(const std::function<void()>& okFunc, const std::function<void()>& cancelFunc);
        static void YesNo(const std::function<void()>& yesFunc, const std::function<void()>& noFunc);
        static void YesNoCancel(const std::function<void()>& yesFunc, const std::function<void()>& noFunc, const std::function<void()>& cancelFunc);

    private:
        static bool shouldInvertOrder();
    };
}

#endif
