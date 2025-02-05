
#ifndef ENGINE_SCENEHIERARCHY_H
#define ENGINE_SCENEHIERARCHY_H

#include "../../../include/archetypes/entity.h"
class SceneHierarchy {
public:
    void Show();
private:
    void DisplayEntityNode(Entity* entity, int index);
    void CreateEntity(const std::string &name, Entity *parent = nullptr);
};

#endif //ENGINE_SCENEHIERARCHY_H
