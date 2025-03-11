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

#ifndef MARMALADE_MARDOWNPARSER_H
#define MARMALADE_MARDOWNPARSER_H

#include <imgui.h>
#include <imgui_markdown.h>

#include <string>
#include <filesystem>
#include <functional>
#include <utility>

namespace Marmalade::GUI::Components {

    struct MarkdownParserOptions {
        std::filesystem::path BaseDir{};
        int ImgMaxWidth{100};
        int ImgMaxHeight{100};
        std::function<void(std::string)> LinkCallback{nullptr};
    };

    class MarkdownParser {
    public:
        MarkdownParser(std::string markdown, MarkdownParserOptions options) : _markdown(std::move(markdown)), _options(std::move(options)) {}

        void Render();

    private:
        std::string _markdown;
        MarkdownParserOptions _options;

        static ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData data);
        static void linkCallback(ImGui::MarkdownLinkCallbackData data);

        static void resizeImage(int maxWidth, int maxHeight, int& imgWidth, int& imgHeight);
    };
}


#endif
