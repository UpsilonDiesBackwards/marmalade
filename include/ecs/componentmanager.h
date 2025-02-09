#ifndef MARMALADE_COMPONENTMANAGER_H
#define MARMALADE_COMPONENTMANAGER_H

#include "component.h"

class ComponentManager {
public:
    std::vector<std::shared_ptr<Component>> components{};

    void AddComponent(std::shared_ptr<Component> component);

    void RemoveComponent(Component* component);

    std::shared_ptr<Component> GetComponent();
};


#endif
