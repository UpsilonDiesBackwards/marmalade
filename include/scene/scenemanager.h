
#ifndef ENGINE_SCENEMANAGER_H
#define ENGINE_SCENEMANAGER_H


#include <memory>
#include <unordered_map>
#include "scene.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void AddScene(std::shared_ptr<Scene> scene);
    void RemoveScene(std::shared_ptr<Scene> scene);

    std::shared_ptr<Scene> GetSceneByName(const std::string& name) const;
    std::shared_ptr<Scene> GetCurrentScene() const;

    void SetCurrentScene(const std::string& name);
private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currentScene;
};


#endif //ENGINE_SCENEMANAGER_H
