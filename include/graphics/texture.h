
#ifndef MARMALADE_TEXTURE_H
#define MARMALADE_TEXTURE_H


#include <string>
#include "glad/glad.h"

class Texture {
public:
    static GLuint LoadTexture(const std::string &filePath);
};


#endif //MARMALADE_TEXTURE_H
