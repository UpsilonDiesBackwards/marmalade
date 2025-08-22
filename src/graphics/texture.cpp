/*
 * Marmalade - Lightweight Game Engine
 * Copyright (C) 2025 Tayler Parsons
 * Copyright (C) 2025 Ryan Bester
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <graphics/texture.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <iostream>

GLuint Texture::LoadTexture(const std::string& filePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Apply settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data;
    if (filePath.empty()) {
        data = stbi_load("res/textures/UVFallback.png", &width, &height, &channels, 4);
    } else {
        data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    }

    if (data) {
        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        LOG_ERROR("Failed to load texture from file: {}", filePath);
        return 0;
    }

    stbi_image_free(data);

    return texture;
}

GLuint Texture::LoadTexture(const std::string& filePath, const Marmalade::Material::TextureSettings& settings) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Apply settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.magFilter);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data;
    if (filePath.empty()) {
        data = stbi_load("res/textures/UVFallback.png", &width, &height, &channels, 4);
    } else {
        data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    }

    if (data) {
        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        LOG_ERROR("Failed to load texture from file: {}", filePath);
        return 0;
    }

    stbi_image_free(data);

    return texture;
}

glm::uvec2 Texture::GetDimensions(const std::string& filePath) {
    int width, height, channels;

    if (stbi_info(filePath.c_str(), &width, &height, &channels)) {
        return glm::uvec2{width, height};
    } else {
        LOG_ERROR("Failed to get dimensions for image texture file: {}", filePath);
    }

    return glm::uvec2{0, 0};
}
