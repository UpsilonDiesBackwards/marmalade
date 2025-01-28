
#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H


#include <memory>
#include "../../include/archetypes/entity.h"

class Scene {
public:
    Scene(const std::string& name);
    ~Scene();

    void AddEntity(std::shared_ptr<Entity> entity);
    void RemoveEntity(std::shared_ptr<Entity> entity);

    std::vector<std::shared_ptr<Entity>>& GetEntities();

    void Render();
    void Update(float deltaTime);

    const std::string& GetName() const;
private:
    std::string name;
    std::vector<std::shared_ptr<Entity>> entities;
};


#endif //ENGINE_SCENE_H
