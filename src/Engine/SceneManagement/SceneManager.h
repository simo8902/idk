//
// Created by Simeon on 3/17/2025.
//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <memory>

#include "Entity.h"
#include "GameObject.h"

class SceneManager {
public:
    static SceneManager& getInstance() {
        static SceneManager instance;
        return instance;
    }

    void addObject(const std::shared_ptr<GameObject>& obj) {
        if (obj && std::find(rootObjects.begin(), rootObjects.end(), obj) == rootObjects.end()) {
            rootObjects.push_back(obj);
        }
    }

    void addObject(const std::shared_ptr<Entity>& ent) {
        if (ent) {
            // Add the Entity's GameObject to the scene
            auto gameObject = ent->getComponent();
            if (gameObject && std::find(rootObjects.begin(), rootObjects.end(), gameObject) == rootObjects.end()) {
                rootObjects.push_back(gameObject);
            }
        }
    }

    void removeObject(const std::shared_ptr<GameObject>& obj) {
        auto it = std::find(rootObjects.begin(), rootObjects.end(), obj);
        if (it != rootObjects.end()) {
            rootObjects.erase(it);
        }
    }

    const std::vector<std::shared_ptr<GameObject>>& getRootObjects() const {
        return rootObjects;
    }
    const std::vector<std::shared_ptr<Entity>>& getAllEntities() const {
        return entities;
    }
private:
    SceneManager() = default;
    std::vector<std::shared_ptr<GameObject>> rootObjects;
    std::vector<std::shared_ptr<Entity>> entities;
};

#endif //SCENEMANAGER_H
