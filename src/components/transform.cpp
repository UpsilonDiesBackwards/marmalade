
#include <glm/gtc/type_ptr.hpp>
#include "transform.h"
#include <imgui.h>

#include "../../include/archetypes/entity.h"

void Transform::Display(Entity* entity) {
        ImGui::Text(name.c_str());

        // Transform
        if (ImGui::InputFloat2(("Position##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->pos))) {
            entity->setPosition(glm::vec2(entity->transform->pos[0], entity->transform->pos[1]));
        }
        if (ImGui::SliderFloat(("Rotation##" + std::to_string(entity->id)).c_str(), &entity->transform->rotation, 0, 360)) {
            entity->setRotation(entity->transform->rotation);
        }
        if (ImGui::InputFloat2(("Scale##" + std::to_string(entity->id)).c_str(), glm::value_ptr(entity->transform->scale))) {
            entity->setScale(glm::vec2(entity->transform->scale[0], entity->transform->scale[1]));
        }
}

void Transform::Apply(Entity* entity) {
}