
#include <fstream>
#include <iostream>
#include "../gui/stylemanager.h"
#include "imgui.h"

StyleManager::StyleManager() { }

void StyleManager::LoadStyle(const std::string& stylePath) {
    ImGuiStyle& style = ImGui::GetStyle();
    std::ifstream file(stylePath);

    if (!file.is_open()) {
        std::cerr << "Error! Could not open style file: " << stylePath << std::endl;
        return;
    }

    std::string token;
    while (file >> token) {
        if (token == "Color") {
            int index;
            float x, y, z, w;
            file >> index >> x >> y >> z >> w;
            if (index >= 0 && index < ImGuiCol_COUNT) {
                style.Colors[index] = ImVec4(x, y, z, w);
            }
        } else if (token == "Alpha") file >> style.Alpha;
        else if (token == "DisabledAlpha") file >> style.DisabledAlpha;
        else if (token == "WindowPadding") file >> style.WindowPadding.x >> style.WindowPadding.y;
        else if (token == "WindowRounding") file >> style.WindowRounding;
        else if (token == "WindowBorderSize") file >> style.WindowBorderSize;
        else if (token == "WindowMinSize") file >> style.WindowMinSize.x >> style.WindowMinSize.y;
        else if (token == "WindowTitleAlign") file >> style.WindowTitleAlign.x >> style.WindowTitleAlign.y;
        else if (token == "ChildRounding") file >> style.ChildRounding;
        else if (token == "ChildBorderSize") file >> style.ChildBorderSize;
        else if (token == "PopupRounding") file >> style.PopupRounding;
        else if (token == "PopupBorderSize") file >> style.PopupBorderSize;
        else if (token == "FramePadding") file >> style.FramePadding.x >> style.FramePadding.y;
        else if (token == "FrameRounding") file >> style.FrameRounding;
        else if (token == "FrameBorderSize") file >> style.FrameBorderSize;
        else if (token == "ItemSpacing") file >> style.ItemSpacing.x >> style.ItemSpacing.y;
        else if (token == "ItemInnerSpacing") file >> style.ItemInnerSpacing.x >> style.ItemInnerSpacing.y;
        else if (token == "CellPadding") file >> style.CellPadding.x >> style.CellPadding.y;
        else if (token == "TouchExtraPadding") file >> style.TouchExtraPadding.x >> style.TouchExtraPadding.y;
        else if (token == "IndentSpacing") file >> style.IndentSpacing;
        else if (token == "ColumnsMinSpacing") file >> style.ColumnsMinSpacing;
        else if (token == "ScrollbarSize") file >> style.ScrollbarSize;
        else if (token == "ScrollbarRounding") file >> style.ScrollbarRounding;
        else if (token == "GrabMinSize") file >> style.GrabMinSize;
        else if (token == "GrabRounding") file >> style.GrabRounding;
        else if (token == "LogSliderDeadzone") file >> style.LogSliderDeadzone;
        else if (token == "TabRounding") file >> style.TabRounding;
        else if (token == "TabBorderSize") file >> style.TabBorderSize;
        else if (token == "TabMinWidthForCloseButton") file >> style.TabMinWidthForCloseButton;
        else if (token == "DisplayWindowPadding") file >> style.DisplayWindowPadding.x >> style.DisplayWindowPadding.y;
        else if (token == "DisplaySafeAreaPadding") file >> style.DisplaySafeAreaPadding.x >> style.DisplaySafeAreaPadding.y;
        else if (token == "MouseCursorScale") file >> style.MouseCursorScale;
        else if (token == "AntiAliasedLines") file >> style.AntiAliasedLines;
        else if (token == "AntiAliasedLinesUseTex") file >> style.AntiAliasedLinesUseTex;
        else if (token == "AntiAliasedFill") file >> style.AntiAliasedFill;
        else if (token == "CurveTessellationTol") file >> style.CurveTessellationTol;
        else if (token == "CircleTessellationMaxError") file >> style.CircleTessellationMaxError;
    }

    file.close();
    std::cout << "Style loaded from: " << stylePath << std::endl;
}

void StyleManager::SaveStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    std::ofstream file(stylePath);

    if (!file.is_open()) {
        std::cerr << "Could not open style file for writing: " << stylePath << std::endl;
        return;
    }

    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        const ImVec4& color = style.Colors[i];
        file << "Color " << i << " " << color.x << " " << color.y << " " << color.z << " " << color.w << "\n";
    }

    file << "Alpha " << style.Alpha << "\n";
    file << "DisabledAlpha " << style.DisabledAlpha << "\n";
    file << "WindowPadding " << style.WindowPadding.x << " " << style.WindowPadding.y << "\n";
    file << "WindowRounding " << style.WindowRounding << "\n";
    file << "WindowBorderSize " << style.WindowBorderSize << "\n";
    file << "WindowMinSize " << style.WindowMinSize.x << " " << style.WindowMinSize.y << "\n";
    file << "WindowTitleAlign " << style.WindowTitleAlign.x << " " << style.WindowTitleAlign.y << "\n";
    file << "WindowMenuButtonPosition " << style.WindowMenuButtonPosition << "\n";
    file << "ChildRounding " << style.ChildRounding << "\n";
    file << "ChildBorderSize " << style.ChildBorderSize << "\n";
    file << "PopupRounding " << style.PopupRounding << "\n";
    file << "PopupBorderSize " << style.PopupBorderSize << "\n";
    file << "FramePadding " << style.FramePadding.x << " " << style.FramePadding.y << "\n";
    file << "FrameRounding " << style.FrameRounding << "\n";
    file << "FrameBorderSize " << style.FrameBorderSize << "\n";
    file << "ItemSpacing " << style.ItemSpacing.x << " " << style.ItemSpacing.y << "\n";
    file << "ItemInnerSpacing " << style.ItemInnerSpacing.x << " " << style.ItemInnerSpacing.y << "\n";
    file << "CellPadding " << style.CellPadding.x << " " << style.CellPadding.y << "\n";
    file << "TouchExtraPadding " << style.TouchExtraPadding.x << " " << style.TouchExtraPadding.y << "\n";
    file << "IndentSpacing " << style.IndentSpacing << "\n";
    file << "ColumnsMinSpacing " << style.ColumnsMinSpacing << "\n";
    file << "ScrollbarSize " << style.ScrollbarSize << "\n";
    file << "ScrollbarRounding " << style.ScrollbarRounding << "\n";
    file << "GrabMinSize " << style.GrabMinSize << "\n";
    file << "GrabRounding " << style.GrabRounding << "\n";
    file << "LogSliderDeadzone " << style.LogSliderDeadzone << "\n";
    file << "TabRounding " << style.TabRounding << "\n";
    file << "TabBorderSize " << style.TabBorderSize << "\n";
    file << "TabMinWidthForCloseButton " << style.TabMinWidthForCloseButton << "\n";
    file << "DisplayWindowPadding " << style.DisplayWindowPadding.x << " " << style.DisplayWindowPadding.y << "\n";
    file << "DisplaySafeAreaPadding " << style.DisplaySafeAreaPadding.x << " " << style.DisplaySafeAreaPadding.y << "\n";
    file << "MouseCursorScale " << style.MouseCursorScale << "\n";
    file << "AntiAliasedLines " << style.AntiAliasedLines << "\n";
    file << "AntiAliasedLinesUseTex " << style.AntiAliasedLinesUseTex << "\n";
    file << "AntiAliasedFill " << style.AntiAliasedFill << "\n";
    file << "CurveTessellationTol " << style.CurveTessellationTol << "\n";
    file << "CircleTessellationMaxError " << style.CircleTessellationMaxError << "\n";

    file.close();
    std::cout << "Style saved to: " << stylePath << std::endl;
}