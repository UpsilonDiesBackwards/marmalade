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

#include "messagebox.h"

#include "../windowmanager.h"
#include "../../application/plugins/interfaceimpl.h"

#define CALLBACK_WITH_RESULT(result) \
    Result res{ResultType_##result}; \
    _dialog->Callback(true, &res);   \
    visible = false;

#define CALLBACK_WITH_CANCEL()      \
    Result res{ResultType_CANCEL};  \
    _dialog->Callback(false, &res); \
    visible = false;

std::string Marmalade::GUI::MsgBox::GetName() {
    return _title;
}

void Marmalade::GUI::MsgBox::Draw() {
    ImGui::SetNextWindowSizeConstraints(_dialog->MinSize, ImVec2(FLT_MAX, FLT_MAX));

    WINDOW_BEGIN_MODAL(GetName().c_str(), ImGuiWindowFlags_AlwaysAutoResize)

    ImGui::Text(_message.c_str());

    switch (_buttons) {
        case Buttons_OK:
            DialogButtons::Ok([&] {
                CALLBACK_WITH_RESULT(OK)
            });
            break;
        case Buttons_OK_CANCEL:
            DialogButtons::OkCancel([&] { CALLBACK_WITH_RESULT(OK) }, [&] { CALLBACK_WITH_CANCEL() });
            break;
        case Buttons_YES_NO:
            DialogButtons::YesNo([&] { CALLBACK_WITH_RESULT(YES) }, [&] { CALLBACK_WITH_RESULT(NO) });
            break;
        case Buttons_YES_NO_CANCEL:
            DialogButtons::YesNoCancel([&] { CALLBACK_WITH_RESULT(YES) }, [&] { CALLBACK_WITH_RESULT(NO) }, [&] { CALLBACK_WITH_CANCEL() });
            break;
    }

    WINDOW_END_MODAL()
}

void Marmalade::GUI::MsgBox::ShowMsgBox(const std::string& title, const std::string& message, Buttons buttons, std::function<void(Result*)> callback) {
    auto dlg = WindowManager::GetInstance().RegisterDialog(std::make_shared<MsgBox>(title, message, buttons), [callback](bool, void* data) {
                        auto *res = static_cast<Result*>(data);
                        if (callback != nullptr) callback(res);
    }, true, ImGuiWindowFlags_NoCollapse, ImVec2(200, 60));
    WindowManager::GetInstance().ShowDialog(dlg.Name);
}
