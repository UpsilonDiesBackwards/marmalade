
#include <algorithm>
#include <iostream>
#include "../../include/scene/scene.h"

Scene::Scene(const std::string& name) : name(name) {
}

Scene::~Scene() {
}

void Scene::AddEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::find(entities.begin(), entities.end(), entity));
}

void Scene::RemoveEntity(Entity* entity) {
    auto it = std::find_if(entities.begin(), entities.end(), [entity](const std::shared_ptr<Entity>& e) {
        return e.get() == entity;
    });

    if (it != entities.end()) {
        entities.erase(it);
    }
}

std::vector<std::shared_ptr<Entity>>& Scene::GetEntities() {
    return entities;
}

void Scene::Render() {
    for (const auto& entity: entities) {
        entity->Render();
    }
}

const std::string& Scene::GetName() const {
    return name;
}