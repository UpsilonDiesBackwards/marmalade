#include "packagemanager.h"
#include "imgui.h"

void PackageManager::Show(bool* p_open) {
    ImGui::Begin("Package Manager", p_open, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

    ImGui::End();
}
