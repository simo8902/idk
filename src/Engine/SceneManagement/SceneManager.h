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

    void addEntity(const std::shared_ptr<Entity>& ent) {
        if (ent && std::ranges::find(entities, ent) == entities.end()) {
            entities.push_back(ent);
        }
    }

    void removeEntity(const std::shared_ptr<Entity>& ent) {
        auto it = std::ranges::find(entities, ent);
        if (it != entities.end()) {
            entities.erase(it);
        }
    }

    const std::vector<std::shared_ptr<Entity>>& getEntities() const {
        return entities;
    }

private:
    SceneManager() = default;
    std::vector<std::shared_ptr<Entity>> entities;
};

#endif //SCENEMANAGER_H
