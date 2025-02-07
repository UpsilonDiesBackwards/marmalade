
#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <memory>
#include <algorithm>
#include "../../include/graphics/renderable.h"

#include "componentmanager.h"
#include "../../src/components/transform.h"
#include "../../src/components/spriterender.h"

enum EntityFlags {
    ACTIVE = 1 << 0,
    HAS_BOUNDS = 1 << 2,
    PLAYER_CONTROLLED = 1 << 3,
    RENDERABLE = 1 << 4,
    NONE,
};

struct Entity {
public:
    unsigned int id;

    std::string name;
    EntityFlags flags;
    Renderable renderable;
    ComponentManager componentManager{};

    // Temp
    std::shared_ptr<Transform> transform = std::make_shared<Transform>();
    std::shared_ptr<SpriteRender> spriteRender = std::make_shared<SpriteRender>();

    Entity* parent{nullptr};
    std::vector<std::unique_ptr<Entity>> children;

    Entity(const std::string& name, EntityFlags flags);

    glm::vec2 getPosition();
    void setPosition(glm::vec2 newPos);

    float getRotation();
    void setRotation(float newRot);

    glm::vec2 getScale();
    void setScale(glm::vec2 newScale);

    void UpdateModelMatrix();

    void AddChild(std::unique_ptr<Entity> child);
    void RemoveChild(Entity* child);

    bool HasParent() const;
    void SetParent(Entity* newParent);

    void Render();
};

#endif //ENGINE_ENTITY_H
