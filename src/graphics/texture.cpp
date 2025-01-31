
#include <iostream>
#include "../../include/graphics/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLuint Texture::LoadTexture(const std::string &filePath) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
//    stbi_set_flip_vertically_on_load(true);

    unsigned char *data;
    if (filePath == "") { // if not file path is defined, use default fallback texture
        data = stbi_load("res/textures/UVFallback.png", &width, &height, &channels, 0);
    } else { // if path is defined then use the given path
        data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    }

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture from file: " + filePath << std::endl;
    }
    stbi_image_free(data);

    return texture;
}
