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

#include "markdownparser.h"

#include "../../application/util.h"
#include "../fontmanager.h"

void Marmalade::GUI::Components::MarkdownParser::Render() {
    ImGui::MarkdownConfig config{
            .linkCallback = &linkCallback,
            .imageCallback = &imageCallback,
            .userData = &_options,
            .formatCallback = &formatCallback};

    ImGui::Markdown(_markdown.c_str(), _markdown.length(), config);
}

void Marmalade::GUI::Components::MarkdownParser::formatCallback(const ImGui::MarkdownFormatInfo& info, bool start) {
    ImGui::defaultMarkdownFormatCallback(info, start);

    switch (info.type) {
        case ImGui::MarkdownFormatType::HEADING: {
            switch (info.level) {
                case 1:
                    if (start) {
                        ImGui::PushFont(Marmalade::GUI::FontManager::GetInstance().fontTitle);
                    } else {
                        ImGui::PopFont();
                    }
                    break;
                case 2:
                    if (start) {
                        ImGui::PushFont(Marmalade::GUI::FontManager::GetInstance().fontHeading);
                    } else {
                        ImGui::PopFont();
                    }
                    break;
                case 3:
                    if (start) {
                        ImGui::PushFont(Marmalade::GUI::FontManager::GetInstance().fontSubheading);
                    } else {
                        ImGui::PopFont();
                    }
                    break;
            }
        }
        default:
            break;
    }
}

ImGui::MarkdownImageData Marmalade::GUI::Components::MarkdownParser::imageCallback(ImGui::MarkdownLinkCallbackData data) {
    auto* options = static_cast<MarkdownParserOptions*>(data.userData);
    auto imgPath = std::string(data.link).substr(0, data.linkLength);

    int width, height;
    ImTextureID image = Marmalade::Util::LoadGuiTexture((options->BaseDir / imgPath).string(), &width, &height);

    resizeImage(options->ImgMaxWidth, options->ImgMaxHeight, width, height);

    ImGui::MarkdownImageData imageData{
            .isValid = true,
            .useLinkCallback = true,
            .user_texture_id = image,
            .size = ImVec2(width, height),
            .uv0 = ImVec2(0, 1),
            .uv1 = ImVec2(1, 0)};

    return imageData;
}

void Marmalade::GUI::Components::MarkdownParser::linkCallback(ImGui::MarkdownLinkCallbackData data) {
    auto* options = static_cast<MarkdownParserOptions*>(data.userData);
    auto url = std::string(data.link).substr(0, data.linkLength);

    if (options->LinkCallback != nullptr) {
        options->LinkCallback(url);
    } else {
        Util::OpenLink(url);
    }
}

void Marmalade::GUI::Components::MarkdownParser::resizeImage(int maxWidth, int maxHeight, int& imgWidth, int& imgHeight) {
    float scaleX = (float) maxWidth / imgWidth;
    float scaleY = (float) maxHeight / imgHeight;

    float scale = std::min(scaleX, scaleY);

    imgWidth *= scale;
    imgHeight *= scale;
}
