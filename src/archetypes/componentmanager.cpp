
#include "../../include/archetypes/componentmanager.h"

void ComponentManager::AddComponent(std::shared_ptr<Component> component) {
    components.push_back(component);
}

void ComponentManager::RemoveComponent(Component* component) {
    components.erase(
                std::remove_if(components.begin(), components.end(),
                               [component](const std::shared_ptr<Component>& comp) {
                                   return comp.get() == component;
                               }),
                components.end()
                );
}

std::shared_ptr<Component> ComponentManager::GetComponent() {
    return std::shared_ptr<Component>();
}
