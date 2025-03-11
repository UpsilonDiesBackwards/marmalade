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

#include "util.h"

#include <stb/stb_image.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <spdlog/spdlog.h>

#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string Marmalade::Util::GenerateUUIDv4() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> distro(0, 0xFFFFFFFF);

    uint32_t p1 = distro(gen);
    uint32_t p2 = distro(gen);
    uint32_t p3 = distro(gen);
    uint32_t p4 = distro(gen);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << p1 << "-";
    ss << std::setw(4) << (p2 >> 16) << "-";
    ss << std::setw(4) << ((p2 & 0x0FFF) | 0x4000) << "-";// version 4
    ss << std::setw(4) << ((p3 & 0x3FFF) | 0x8000) << "-";// variant 1
    ss << std::setw(12) << p4;

    return ss.str();
}

std::string Marmalade::Util::StringToLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

void Marmalade::Util::DisplayFile(const std::string& path) {
#ifdef _WIN32
    ShellExecute(nullptr, "open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif __APPLE__
    std::string command = "open " + path + " &";
    std::system(command.c_str());
#else
    std::string command = "xdg-open " + path + " &";
    std::system(command.c_str());
#endif
}

void Marmalade::Util::OpenLink(const std::string& link) {
    DisplayFile(link);
}

GLuint Marmalade::Util::LoadGuiTexture(std::string path) {
    int width, height, channels;
    unsigned char* data = ::stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        spdlog::error("Failed to load texture: {}", path);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
