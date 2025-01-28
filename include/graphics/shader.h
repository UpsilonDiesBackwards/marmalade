
#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include <string>
#include "glm/glm.hpp"

class Shader {
public:
    unsigned int ID;

    Shader(const char* vPath, const char* fPath);

    void Use();

    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void GetError(unsigned int shader, std::string type);
};


#endif //ENGINE_SHADER_H
