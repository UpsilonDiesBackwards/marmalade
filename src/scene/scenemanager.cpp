
#include <iostream>
#include "../../include/scene/scenemanager.h"

SceneManager::SceneManager() : currentScene(nullptr) {
}

SceneManager::~SceneManager() {
    scenes.clear();
}

void SceneManager::AddScene(std::shared_ptr<Scene> scene) {
    const std::string& name = scene->GetName();
    if (scenes.find(name) != scenes.end()) {
        std::cout << "Scene already exists!" << std::endl;
    }

    scenes[name] = scene;
}

void SceneManager::RemoveScene(std::shared_ptr<Scene> scene) {
    const std::string& name = scene->GetName();
    auto targetScene = scenes.find(name);
    if (targetScene != scenes.end()) {
        if (currentScene == targetScene->second) {
            currentScene = nullptr;
        }
        scenes.erase(targetScene);
    }
}

std::shared_ptr<Scene> SceneManager::GetCurrentScene() const {
    return currentScene;
}

void SceneManager::SetCurrentScene(const std::string &name) {
    auto scene = scenes.find(name);
    if (scene != scenes.end()) {
        currentScene = scene->second;
    }
}