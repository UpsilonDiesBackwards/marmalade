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

#include "fontmanager.h"

#include "../application/config.h"

#include <glad/glad.h>

#include <IconsCodicons.h>

#ifdef _WIN32
#include <windows.h>
#include <ShellScalingApi.h>
#endif

Marmalade::GUI::FontManager& Marmalade::GUI::FontManager::GetInstance() {
    static FontManager instance{};
    return instance;
}

void Marmalade::GUI::FontManager::InitFonts() {
    auto& io = ImGui::GetIO();

    auto scaleFactor = Config::engineConfig.appearance.scaleFactor;
    if (Config::engineConfig.appearance.useSystemScaleFactor) scaleFactor = getMonitorScale();

    fontTitle = io.Fonts->AddFontFromFileTTF("res/fonts/Cabin/static/Cabin-Regular.ttf", 34 * scaleFactor);
    fontHeading = io.Fonts->AddFontFromFileTTF("res/fonts/Cabin/static/Cabin-Regular.ttf", 28 * scaleFactor);
    fontSubheading = io.Fonts->AddFontFromFileTTF("res/fonts/Cabin/static/Cabin-Regular.ttf", 22 * scaleFactor);
    fontNormal = io.Fonts->AddFontFromFileTTF("res/fonts/Cabin/static/Cabin-Regular.ttf", 16 * scaleFactor);

    io.FontDefault = fontNormal;

    static const ImWchar icons_ranges[] = {ICON_MIN_CI, ICON_MAX_16_CI, 0};

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    float font_size = 16.0f * scaleFactor;
    icons_config.GlyphMinAdvanceX = font_size;
    icons_config.GlyphOffset.y = 3.0f * scaleFactor;
    io.Fonts->AddFontFromFileTTF("res/fonts/codicon.ttf", font_size, &icons_config, icons_ranges);

    rebuildFontAtlas(io, scaleFactor);
}

float Marmalade::GUI::FontManager::getMonitorScale() {
    float scale = 1.0f;

#if defined(WIN32)
    auto primary_monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    UINT dpi_x = 0, dpi_y = 0;

    auto result = GetDpiForMonitor(primary_monitor, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
    if (result == S_OK) {
        scale = static_cast<float>(dpi_x) / 96.0f;
    } else {
        scale = 1.0f;
    }
#endif

    return scale;
}

void Marmalade::GUI::FontManager::rebuildFontAtlas(ImGuiIO& io, float scaleFactor) const {
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    GLuint font_texture;
    glGenTextures(1, &font_texture);
    glBindTexture(GL_TEXTURE_2D, font_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    io.Fonts->TexID = font_texture;

    io.DisplayFramebufferScale = ImVec2(scaleFactor, scaleFactor);
}
