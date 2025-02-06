
#include "details.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <IconsCodicons.h>

void Marmalade::GUI::Details::Draw() {
    ImGui::Begin(ICON_CI_SEARCH " Details", &visible );

    ImGui::Text(inspectedEntity->name.c_str());
    ImGui::Separator();


    ImGui::End();
}
