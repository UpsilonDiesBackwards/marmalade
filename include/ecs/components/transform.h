
#ifndef MARMALADE_TRANSFORM_H
#define MARMALADE_TRANSFORM_H

#include "glm/vec2.hpp"
#include "glm/glm.hpp"

#include "../component.h"

class Transform : public Component {
public:
    glm::vec2 pos = {0.0f, 0.0f};
        float rotation = 0.0f;
        glm::vec2 scale = {1.0f, 1.0f};

        glm::mat4 modelMatrix = glm::mat4(1.0f);

    void Apply(Entity* entity) override ;
    void Display(Entity* entity) override;

    Transform() { name = "Transform"; }

    REGISTER_COMPONENT(Transform)
};

#endif//MARMALADE_TRANSFORM_H
