
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "../../include/archetypes/entity.h"
#include "../../include/graphics/texture.h"

Entity::Entity(const std::string &name, EntityFlags flags, Transform transform)
    : name(name), flags(flags), transform(transform), renderable(0,0,0, Texture::LoadTexture("")),
      children(std::make_shared<std::vector<std::unique_ptr<Entity>>>()) { // Lord forgive me for the c h i l d

    renderable.Initialise();

    Render();
}

void Entity::Render() {
    if (RENDERABLE) {
        renderable.Draw(transform.modelMatrix);
    }
}

glm::vec2 Entity::getPosition() {
    return transform.pos;
}

void Entity::setPosition(glm::vec2 newPos) {
    transform.pos = newPos;
    UpdateModelMatrix();
}

float Entity::getRotation() {
    return transform.rotation;
}

void Entity::setRotation(float newRot) {
    transform.rotation = newRot;
    UpdateModelMatrix();
}

glm::vec2 Entity::getScale() {
    return transform.scale;
}

void Entity::setScale(glm::vec2 newScale) {
    transform.scale = newScale;
    UpdateModelMatrix();
}

void Entity::UpdateModelMatrix() {
    transform.modelMatrix = glm::mat4(1.0f);

    transform.modelMatrix = glm::translate(transform.modelMatrix, glm::vec3(transform.pos, 0.0f));
    transform.modelMatrix = glm::rotate(transform.modelMatrix, glm::radians(transform.rotation),
                                        glm::vec3(0.0f, 0.0f, 1.0f));
    transform.modelMatrix = glm::scale(transform.modelMatrix, glm::vec3(transform.scale, 1.0f));
}
