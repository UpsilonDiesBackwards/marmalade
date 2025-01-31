
#ifndef ENGINE_RENDERABLE_H
#define ENGINE_RENDERABLE_H

#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/mat4x4.hpp>
#include "shader.h"

class Renderable {
public:
    Renderable(unsigned int VAO, unsigned int VBO, unsigned int EBO, unsigned int texture);

    void Initialise(); // Buffer Setup
    void Draw(glm::mat4 modelMatrix);

    unsigned int GetTexture();
private:
    unsigned int VAO, VBO, EBO;
    Shader shaderProgram;
    unsigned int texture;
    glm::mat4 projectionMatrix;
};


#endif //ENGINE_RENDERABLE_H
